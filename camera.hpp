//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//
#pragma once

#include <rtr_fwd.hpp>
#include <transform.hpp>
#include <glm/glm.hpp>
#include <render_configs.hpp>
#include <rtr_config.hpp>
#include <atomic>
#include <set>

namespace rtr
{
struct im_plane
{
    float left;
    float right;

    float top;
    float bottom;

    float dist;

    long width;
    long height;

    float pix_w;
    float pix_h;

    im_plane(float left, float right, float top, float bottom, float dist, long im_w, long im_h)
        : left(left), right(right), top(top), bottom(bottom), dist(dist), width(im_w), height(im_h)
    {
        recalc();
    }

    void recalc()
    {
        pix_w = (right - left) / width;
        pix_h = (top - bottom) / height;
    }

    glm::vec3 get_top_left(const transformation& of) const {
        return of.position - of.forward * dist + of.right * left + of.up * top;
    }
};

class camera
{
public:
    using render_type = render_config::ldr_render;

    camera(const glm::vec3& pos, const glm::vec3& up, const glm::vec3 & gaze, const im_plane& p, const std::string& output);

    typename render_config::render_traits<render_type>::image_type
    render(const scene& scene) const;

    const std::string& get_output() const
    {
        return m_output;
    }

    void set_aperture(float distance, float aperture_size);

    void set_samples(std::uint16_t samples);

    void add_flag(const std::string& flag){
        m_flags.emplace(flag);
    }

private:
    std::set<std::string> m_flags;

    std::atomic<uint64_t>* rendered;
    transformation t;
    im_plane plane;
    std::string m_output;
    std::uint16_t sample_count = 100;
    std::uint16_t sample_sqrt = 10;

    float aperture_size = 0;

    friend void render_scanline(const camera& cam, glm::vec3 row_pos, int row, const glm::vec3&, const glm::vec3&, const scene& scn,
                                typename render_config::render_traits<render_type>::image_type::view_t &v);
};
}

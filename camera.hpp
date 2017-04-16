//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//
#pragma once

#include <rtr_fwd.hpp>
#include <transform.hpp>
#include <glm/glm.hpp>
#include <render_configs.hpp>
#include <rtr_config.hpp>

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

    camera(const glm::vec3& pos, const glm::vec3& up, const glm::vec3 & gaze, const im_plane& p, const std::string& output) :
            t{pos, glm::normalize(up), glm::normalize(-gaze), glm::normalize(glm::cross(t.up, t.forward))}, plane{p}, m_output{output} {
        t.up = glm::normalize(glm::cross(t.right, -t.forward));
    }

    typename render_config::render_traits<render_type>::image_type
    render(const scene& scene) const;

    const std::string& get_output() const
    {
        return m_output;
    }
private:
    transformation t;
    im_plane plane;
    std::string m_output;
    std::uint8_t sample_count;
    std::uint8_t sample_sqrt;

    friend void render_scanline(const camera& cam, glm::vec3 row_pos, int row, const glm::vec3&, const glm::vec3&, const scene& scn,
                                typename render_config::render_traits<render_type>::image_type::view_t &v);
};
}

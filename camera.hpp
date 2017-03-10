//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//
#pragma once

#include <rtr_fwd.hpp>
#include <transform.hpp>
#include <glm/glm.hpp>
#include <render_configs.hpp>

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

    glm::vec3 get_top_left(const transform& of) const {
        return of.position - of.forward * dist + of.right * left + of.up * top;
    }
};

class camera
{
    const transform t;
    im_plane plane;

public:
    using render_type = render_config::hdr_render;

    camera(const glm::vec3& pos, const glm::vec3& up, const glm::vec3& gaze, const im_plane& p) :
            t{pos, up, -gaze, glm::cross(up, -gaze)}, plane{p} {}

    typename render_config::render_traits<render_type>::image_type
    render(const scene& scene) const;

    const transform& get_transform() const
    {
        return t;
    }
};
}

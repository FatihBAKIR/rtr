//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//
#pragma once

#include <rtr_fwd.hpp>
#include <transform.hpp>
#include <glm/glm.hpp>
#include <boost/gil/gil_all.hpp>
#include <gil_extension/exr/half/typedefs.hpp>

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
    camera(const glm::vec3& pos, const glm::vec3& up, const glm::vec3& gaze, const im_plane& p) :
            t{pos, up, -gaze, glm::cross(up, -gaze)}, plane{p} {}

    boost::gil::rgb16f_image_t render(const scene& scene) const;

    const transform& get_transform() const
    {
        return t;
    }
};
}

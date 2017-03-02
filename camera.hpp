//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//
#pragma once

#include <transform.hpp>
#include <glm/glm.hpp>

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

    float pixel_width() const {
        return (right - left) / width;
    }

    float pixel_height() const {
        return (top - bottom) / height;
    }

    glm::vec3 get_top_left(const transform& of) {
        return of.position - of.forward * dist + of.right * left + of.up * top;
    }
};

class camera
{
    const transform t;

public:
    camera(const glm::vec3& pos, const glm::vec3& up, const glm::vec3& gaze) :
            t{pos, up, -gaze, glm::cross(up, -gaze)} {}

    const transform& get_transform() const
    {
        return t;
    }
};
}

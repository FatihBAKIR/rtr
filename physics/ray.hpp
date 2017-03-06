//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <rtr_fwd.hpp>
#include <iosfwd>

namespace rtr
{
namespace physics
{
struct ray
{
    glm::vec3 origin;
    glm::vec3 dir;

    constexpr ray(const glm::vec3& origin, const glm::vec3& dir) : origin{origin}, dir{dir} {}

    friend std::ostream& operator<<(std::ostream& os, const ray& r);
};

struct ray_hit
{
    struct ray r;
    const material* mat;
    glm::vec3 position;
    glm::vec3 normal;
    float parameter;
};
}
}


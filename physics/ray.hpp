//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <rtr_fwd.hpp>
#include <iosfwd>
#include <scene.hpp>

namespace rtr
{
namespace physics
{
struct ray
{
    glm::vec3 origin;
    glm::vec3 dir;
    mutable glm::vec3 m_inverse;

    constexpr ray(const glm::vec3& origin, const glm::vec3& dir) : origin{origin}, dir{dir} {
        m_inverse = glm::vec3(1.f, 1.f, 1.f) / dir;
    }

    friend std::ostream& operator<<(std::ostream& os, const ray& r);

    glm::vec3 get_inverse() const {
        return m_inverse;
    }
};

struct ray_hit
{
    scene::shape_ptr_variant shape;
    struct ray r;
    const material* mat;
    glm::vec3 position;
    glm::vec3 normal;
    float parameter;
};
}
}


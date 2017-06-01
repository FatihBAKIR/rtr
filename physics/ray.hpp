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
    std::uint8_t rtl; // reflections to live, if 0, discard
    glm::vec3 m_inverse;
    std::uint16_t ms_id; // multisampling id
    bool m_backface_cull = false;

    ray(const glm::vec3& origin, const glm::vec3& dir) : origin{origin}, dir{dir}, m_inverse{glm::vec3(1.f, 1.f, 1.f) / dir} {}

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

    glm::vec2 uv = {};
    glm::vec3 dpdu, dpdv;
};
}
}


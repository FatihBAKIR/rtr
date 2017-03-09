//
// Created by fatih on 09.03.2017.
//

#include <glm/vec3.hpp>
#include <material.hpp>
#include <scene.hpp>
#include <lights.hpp>

namespace rtr
{
    glm::vec3 material::calculate_color(const scene* scene, const glm::vec3& pos, const glm::vec3 &normal) const {
        return (normal + glm::vec3(1, 1, 1)) * 0.5f;
    }
}

//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <rtr_fwd.hpp>

namespace rtr
{
    struct material
    {
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;

        float phong;

        glm::vec3 calculate_color(const scene* scene, const glm::vec3& pos, const glm::vec3& normal) const;

        long id;
    };
}
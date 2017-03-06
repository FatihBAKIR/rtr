//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <glm/vec3.hpp>

namespace rtr
{
    struct material
    {
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;

        float phong;

        long id;
    };
}
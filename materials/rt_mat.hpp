//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <rtr_fwd.hpp>
#include <materials/material.hpp>

namespace rtr
{
    class rt_mat : public material
    {
    public:
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;

        float phong;

        glm::vec3 shade(const shading_ctx& ctx) const override final;
    };
}
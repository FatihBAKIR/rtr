#pragma once
//
// Created by fatih on 5/13/17.
//

#include <glm/vec3.hpp>

namespace rtr
{
namespace brdf
{
    struct brdf_data
    {
        glm::vec3 diffuse;
        glm::vec3 specular;
        glm::vec3 ambient;
    };

    struct brdf_ctx
    {
        const shading_ctx& s_ctx;
        glm::vec3 normal_ptol;
    };
}
}

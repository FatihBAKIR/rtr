//
// Created by fatih on 09.04.2017.
//

#pragma once


#include <materials/material.hpp>

namespace rtr
{
namespace shading
{
    class glass : public material
    {
        float refract_index;

    public:
        glm::vec3 shade(const shading_ctx &ctx) const override;
    };
}
}


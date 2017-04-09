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
        glm::vec3 attenuate;

    public:
        glass(const glm::vec3& trans, float index) :
            refract_index(index), attenuate(trans) {}

        glm::vec3 shade(const shading_ctx &ctx) const override;
    };
}
}


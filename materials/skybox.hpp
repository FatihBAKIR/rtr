//
// Created by fatih on 6/11/17.
//

#pragma once

#include <materials/material.hpp>

namespace rtr
{
namespace shading
{
    class skybox : public material
    {
        const texturing::sampler2d* m_light_probe;
        float m_scale = 1;

    public:
        skybox(const texturing::sampler2d* l_probe, float scaling = 1) : m_light_probe(l_probe), m_scale(scaling) {}

        glm::vec3 shade(const shading_ctx& ctx) const override;
    };
}
}


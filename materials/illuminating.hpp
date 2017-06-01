//
// Created by fatih on 6/1/17.
//

#pragma once

#include <materials/material.hpp>

namespace rtr
{
namespace shading
{
    class illuminating : public material
    {
        glm::vec3 m_intensity;
    public:
        illuminating(const glm::vec3&);

        glm::vec3 shade(const shading_ctx& ctx) const override;
    };
}
}



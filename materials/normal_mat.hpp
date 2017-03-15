//
// Created by fatih on 12.03.2017.
//

#pragma once

#include <materials/material.hpp>

namespace rtr
{
    class normal_mat : public material {
    public:
        glm::vec3 shade(const shading_ctx &ctx) const override final;
    };
}

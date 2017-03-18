//
// Created by fatih on 12.03.2017.
//

#pragma once

#include <materials/shading_ctx.hpp>

namespace rtr
{
    class material
    {
    public:
        virtual glm::vec3 shade(const shading_ctx&) const = 0;
        virtual ~material() = default;
        short id;
    };
}

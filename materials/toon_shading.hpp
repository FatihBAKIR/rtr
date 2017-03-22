//
// Created by fatih on 18.03.2017.
//

#pragma once

#include <rtr_fwd.hpp>
#include <materials/material.hpp>
#include <vector>

namespace rtr
{
namespace shading
{
class toon_shader: public material {
    const material* base_mat;
    std::vector<glm::vec3> quantized;

public:
    glm::vec3 shade(const shading_ctx &ctx) const override;
};
}
}


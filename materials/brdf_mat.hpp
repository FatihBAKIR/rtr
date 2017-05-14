//
// Created by fatih on 5/13/17.
//

#pragma once

#include <materials/material.hpp>
#include <brdf/brdf_common.hpp>

namespace rtr
{
namespace shading
{
    template <class BrdfT>
    class brdf_mat : public material
    {
    public:
        glm::vec3 shade(const shading_ctx& ctx) const override;

    private:
        BrdfT brdf;
        brdf::brdf_data data;
    };
}
}



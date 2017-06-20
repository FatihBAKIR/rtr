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
        brdf_mat() = delete;
        brdf_mat(const brdf::brdf_data& data, const BrdfT& b) : brdf(b), data(data) {}
        glm::vec3 shade(const shading_ctx& ctx) const override;

    private:
        BrdfT brdf;
        brdf::brdf_data data;
    };

    template <class BrdfT>
    material* make_brdf(const brdf::brdf_data& data, const BrdfT& b)
    {
        return new brdf_mat<BrdfT>(data, b);
    }
}
}



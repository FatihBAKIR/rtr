//
// Created by Mehmet Fatih BAKIR on 14/05/2017.
//

#include <algorithm>
#include <materials/shading_ctx.hpp>
#include <physics/ray.hpp>
#include <boost/math/constants/constants.hpp>
#include <iostream>
#include "torrance_sparrow.hpp"

namespace rtr
{
namespace brdf
{
    static constexpr auto pi = boost::math::constants::pi<float>();
    glm::vec3 torrance_sparrow::calculate(const brdf_data* data, const brdf_ctx* ctx) const
    {
        using namespace glm;

        auto half = normalize(ctx->s_ctx.view_dir + ctx->normal_ptol);

        auto cos_theta_h = std::max(0.f, dot(half, ctx->s_ctx.hit.normal));
        auto cos_theta_o = dot(ctx->s_ctx.view_dir, ctx->s_ctx.hit.normal);
        auto cos_theta_i = dot(ctx->normal_ptol, ctx->s_ctx.hit.normal);
        auto cos_alpha_h = dot(half, ctx->s_ctx.view_dir);

        auto G = std::min({
            1.f,
            2.f * cos_theta_h * cos_theta_o / cos_alpha_h,
            2.f * cos_theta_h * cos_theta_i / cos_alpha_h
        });

        auto D = ((exponent + 2) / (2 * pi)) * std::pow(cos_theta_h, exponent);

        auto F = data->specular + (glm::vec3{1, 1, 1} - data->specular) * float(std::pow(1.f - cos_alpha_h, 5.f));

        auto res = D * G * F / (4 * cos_theta_i * cos_theta_o);
        return (data->diffuse / pi + res) * cos_theta_i;
    }
}
}
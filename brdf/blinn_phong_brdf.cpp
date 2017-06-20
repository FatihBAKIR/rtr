//
// Created by Mehmet Fatih BAKIR on 14/05/2017.
//

#include <boost/math/constants/constants.hpp>
#include <glm/glm.hpp>
#include <physics/ray.hpp>
#include "blinn_phong_brdf.hpp"

namespace rtr
{
namespace brdf
{
    static constexpr auto pi = boost::math::constants::pi<float>();

    glm::vec3 blinn_phong::calculate(const brdf_data* data, const brdf_ctx* ctx) const
    {
        auto half = glm::normalize(ctx->s_ctx.view_dir + ctx->normal_ptol);

        auto cos_theta_h = std::max(0.f, glm::dot(half, ctx->s_ctx.hit.normal));
        auto cos_theta_i = glm::dot(ctx->normal_ptol, ctx->s_ctx.hit.normal);

        auto phong_brdf = data->specular * std::pow(cos_theta_h, phong_coeff) / (cos_theta_i);

        return (data->diffuse + phong_brdf) * cos_theta_i;
    }

    glm::vec3 blinn_phong_modified::calculate(const brdf_data* data, const brdf_ctx* ctx) const
    {
        auto half = glm::normalize(ctx->s_ctx.view_dir + ctx->normal_ptol);

        auto cos_theta_h = std::max(0.f, glm::dot(half, ctx->s_ctx.hit.normal));
        auto cos_theta_i = glm::dot(ctx->normal_ptol, ctx->s_ctx.hit.normal);

        auto phong_brdf = data->specular * std::pow(cos_theta_h, phong_coeff);

        return (data->diffuse + phong_brdf) * cos_theta_i;
    }

    glm::vec3 blinn_phong_modified_normalized::calculate(const brdf_data* data, const brdf_ctx* ctx) const
    {
        auto half = glm::normalize(ctx->s_ctx.view_dir + ctx->normal_ptol);

        auto cos_theta_h = std::max(0.f, glm::dot(half, ctx->s_ctx.hit.normal));
        auto cos_theta_i = glm::dot(ctx->normal_ptol, ctx->s_ctx.hit.normal);

        auto phong_brdf = data->specular * ((phong_coeff + 8) / 8.f) * std::pow(cos_theta_h, phong_coeff) / pi;

        return (data->diffuse / pi + phong_brdf) * cos_theta_i;
    }
}
}

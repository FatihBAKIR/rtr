//
// Created by fatih on 5/13/17.
//

#include <boost/math/constants/constants.hpp>
#include <glm/glm.hpp>
#include <physics/ray.hpp>
#include "phong_brdf.hpp"

namespace rtr
{
namespace brdf
{
    static constexpr auto pi = boost::math::constants::pi<float>();

    glm::vec3 phong::calculate(const brdf_data* data, const brdf_ctx* ctx) const
    {
        auto reflect_dir = glm::normalize(
                2.f * glm::dot(ctx->normal_ptol, ctx->s_ctx.hit.normal) * ctx->s_ctx.hit.normal
                        - ctx->normal_ptol);

        auto cos_alpha = std::max(0.f, glm::dot(ctx->s_ctx.view_dir, reflect_dir));
        auto cos_theta_i = glm::dot(ctx->normal_ptol, ctx->s_ctx.hit.normal);

        auto phong_brdf = data->specular * std::pow(cos_alpha, phong_coeff) / (cos_theta_i);

        return (data->diffuse / pi + phong_brdf) * cos_theta_i;
    }

    glm::vec3 phong_modified::calculate(const brdf_data* data, const brdf_ctx* ctx) const
    {
        auto reflect_dir = glm::normalize(
                2.f * glm::dot(ctx->normal_ptol, ctx->s_ctx.hit.normal) * ctx->s_ctx.hit.normal
                        - ctx->normal_ptol);

        auto cos_alpha = std::max(0.f, glm::dot(ctx->s_ctx.view_dir, reflect_dir));
        auto cos_theta_i = glm::dot(ctx->normal_ptol, ctx->s_ctx.hit.normal);

        auto phong_brdf = data->specular * std::pow(cos_alpha, phong_coeff);

        return (data->diffuse / pi + phong_brdf) * cos_theta_i;
    }

    glm::vec3 phong_modified_normalized::calculate(const brdf_data* data, const brdf_ctx* ctx) const
    {
        auto reflect_dir = glm::normalize(
                2.f * glm::dot(ctx->normal_ptol, ctx->s_ctx.hit.normal) * ctx->s_ctx.hit.normal
                        - ctx->normal_ptol);

        auto cos_alpha = std::max(0.f, glm::dot(ctx->s_ctx.view_dir, reflect_dir));
        auto cos_theta_i = glm::dot(ctx->normal_ptol, ctx->s_ctx.hit.normal);

        auto phong_brdf = data->specular * ((phong_coeff + 2) * 0.5f) * std::pow(cos_alpha, phong_coeff) / pi;

        return (data->diffuse / pi + phong_brdf) * cos_theta_i;
    }
}
}


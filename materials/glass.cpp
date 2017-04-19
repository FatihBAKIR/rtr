//
// Created by fatih on 09.04.2017.
//

#include <glm/glm.hpp>
#include <physics/ray.hpp>
#include "glass.h"

static constexpr float AirRefractionIndex = 1.f;

namespace rtr
{
namespace shading
{
    float delta(const glm::vec3& normal, float refract_ratio, const glm::vec3& view_dir)
    {
        auto cos_theta = glm::dot(view_dir, normal);
        return 1.f - std::pow(refract_ratio, 2) * (1 - cos_theta * cos_theta);
    }

    glm::vec3 refract_ray(glm::vec3 normal, float refract_ratio, const glm::vec3& view_dir)
    {
        auto cos_theta = glm::dot(view_dir, normal);
        auto new_dir = refract_ratio * -view_dir;
        new_dir += normal * (refract_ratio * cos_theta - std::sqrt(delta(normal, refract_ratio, view_dir)));
        new_dir = glm::normalize(new_dir);
        return new_dir;
    }

    glm::vec3 rtr::shading::glass::shade(const rtr::shading_ctx &ctx) const {
        glm::vec3 regular;

        if (ctx.hit.r.rtl == 0) return regular;

        float refl_coeff = 0;
        bool is_entering = glm::dot(-ctx.view_dir, ctx.hit.normal) < 0.f;
        if (is_entering)
        {
            auto cos_theta = glm::dot(ctx.view_dir, ctx.hit.normal);
            auto refl_ray = physics::ray(ctx.hit.position + ctx.hit.normal * 0.001f,
                -ctx.view_dir + 2.f * cos_theta * ctx.hit.normal);
            refl_ray.rtl = ctx.hit.r.rtl - std::uint8_t(1);
            refl_ray.ms_id = ctx.hit.r.ms_id;

            auto refl_hit = ctx.scn.ray_cast(refl_ray);
            if (refl_hit)
            {
                auto reflect_ctx = shading_ctx{
                    ctx.scn,
                    -refl_ray.dir,
                    *refl_hit
                };

                auto r0 = std::pow((AirRefractionIndex - refract_index) / (AirRefractionIndex + refract_index), 2);
                refl_coeff = r0 + (1 - r0) * std::pow(1 - cos_theta, 5);

                auto res = refl_hit->mat->shade(reflect_ctx);
                regular += refl_coeff * res;
            }

            auto new_ray = physics::ray(ctx.hit.position - ctx.hit.normal * 0.001f,
                    refract_ray(ctx.hit.normal, AirRefractionIndex / refract_index, ctx.view_dir));
            new_ray.rtl = ctx.hit.r.rtl - std::uint8_t(1);
            new_ray.ms_id = ctx.hit.r.ms_id;

            auto hit = ctx.scn.ray_cast(new_ray);
            if (hit)
            {
                auto reflect_ctx = shading_ctx{
                    ctx.scn,
                    -new_ray.dir,
                    *hit
                };

                auto res = hit->mat->shade(reflect_ctx);
                auto t = hit->parameter;
                auto exponent = glm::vec3(std::log(attenuate[0]), std::log(attenuate[1]), std::log(attenuate[2])) * t;
                regular += (1 - refl_coeff) * res * glm::vec3(std::exp(exponent[0]), std::exp(exponent[1]), std::exp(exponent[2]));
            }
        }
        else
        {
            if (delta(-ctx.hit.normal, refract_index / AirRefractionIndex, ctx.view_dir) >= 0)
            {
                auto cos_theta = glm::dot(ctx.view_dir, -ctx.hit.normal);
                auto refl_ray = physics::ray(ctx.hit.position - ctx.hit.normal * 0.001f,
                    -ctx.view_dir - 2.f * cos_theta * ctx.hit.normal);
                refl_ray.rtl = ctx.hit.r.rtl - std::uint8_t(1);
                refl_ray.ms_id = ctx.hit.r.ms_id;

                auto refl_hit = ctx.scn.ray_cast(refl_ray);
                if (refl_hit)
                {
                    auto reflect_ctx = shading_ctx{
                        ctx.scn,
                        -refl_ray.dir,
                        *refl_hit
                    };

                    auto r0 = std::pow((AirRefractionIndex - refract_index) / (AirRefractionIndex + refract_index), 2);
                    refl_coeff = r0 + (1 - r0) * std::pow(1 - cos_theta, 5);

                    auto res = refl_hit->mat->shade(reflect_ctx);
                    regular += refl_coeff * res;
                }

                auto new_ray = physics::ray(ctx.hit.position + ctx.hit.normal * 0.001f,
                        refract_ray(-ctx.hit.normal, refract_index / AirRefractionIndex, ctx.view_dir));
                new_ray.rtl = ctx.hit.r.rtl - std::uint8_t(1);
                new_ray.ms_id = ctx.hit.r.ms_id;

                auto hit = ctx.scn.ray_cast(new_ray);
                if (hit)
                {
                    auto reflect_ctx = shading_ctx{
                        ctx.scn,
                        -new_ray.dir,
                        *hit
                    };

                    auto res = hit->mat->shade(reflect_ctx);
                    regular += (1 - refl_coeff) * res;
                }
            }
            else // internal reflection
            {
                auto reflect_dir = glm::normalize(2.f * glm::dot(ctx.view_dir, -ctx.hit.normal) * -ctx.hit.normal - ctx.view_dir);
                auto reflect_ray = rtr::physics::ray(ctx.hit.position - ctx.hit.normal * 0.001f, reflect_dir);
                reflect_ray.rtl = ctx.hit.r.rtl - std::uint8_t(1);
                reflect_ray.ms_id = ctx.hit.r.ms_id;
                auto hit = ctx.scn.ray_cast(reflect_ray);
                if (hit)
                {
                    auto reflect_ctx = shading_ctx{
                        ctx.scn,
                        -reflect_dir,
                        *hit
                    };

                    auto res = hit->mat->shade(reflect_ctx);
                    auto t = hit->parameter;
                    auto exponent = glm::vec3(std::log(attenuate[0]), std::log(attenuate[1]), std::log(attenuate[2])) * t;
                    regular += res * glm::vec3(std::exp(exponent[0]), std::exp(exponent[1]), std::exp(exponent[2]));
                }
            }
        }
        return regular;
    }
}
}


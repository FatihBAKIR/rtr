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
    glm::vec3 refract_ray(glm::vec3 normal, float refract_ratio, const glm::vec3& view_dir)
    {
        auto cos_theta = glm::dot(view_dir, normal);
        auto new_dir = refract_ratio * (-view_dir + normal * cos_theta);
        new_dir -= normal * (float)std::sqrt(1.f - std::pow(refract_ratio * (1 - cos_theta * cos_theta), 2));
        new_dir = glm::normalize(new_dir);
        return new_dir;
    }

    glm::vec3 rtr::shading::glass::shade(const rtr::shading_ctx &ctx) const {
        bool is_entering = glm::dot(-ctx.view_dir, ctx.hit.normal) < 0.f;

        glm::vec3 regular;

        if (is_entering)
        {
            auto new_ray = physics::ray(ctx.hit.position + ctx.hit.normal * 0.001, refract_ray(ctx.hit.normal, AirRefractionIndex / refract_index, ctx.view_dir));
            new_ray.rtl = ctx.hit.r.rtl - std::uint8_t(1);
            auto hit = ctx.scn.ray_cast(new_ray);
            if (hit)
            {
                auto reflect_ctx = shading_ctx{
                    ctx.scn,
                    -new_ray.dir,
                    *hit
                };

                auto res = hit->mat->shade(reflect_ctx);
                regular += res;
            }
        }
        else
        {
            auto new_ray = physics::ray(ctx.hit.position + ctx.hit.normal * 0.001, refract_ray(-ctx.hit.normal, refract_index / AirRefractionIndex, ctx.view_dir));
            new_ray.rtl = ctx.hit.r.rtl - std::uint8_t(1);
            auto hit = ctx.scn.ray_cast(new_ray);
        }
    }
}
}


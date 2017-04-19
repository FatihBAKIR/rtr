//
// Created by fatih on 4/19/17.
//

#include <physics/ray.hpp>
#include "metal.hpp"
#include <utility.hpp>

namespace rtr
{
    namespace shading
    {
        glm::vec3 metal::shade(const shading_ctx & ctx) const {
            auto regular = base.shade(ctx);

            if (ctx.hit.r.rtl != 0)
            {
                auto reflect_dir = glm::normalize(2.f * glm::dot(ctx.view_dir, ctx.hit.normal) * ctx.hit.normal - ctx.view_dir);

                auto normal1 = glm::normalize(get_normal(reflect_dir));
                auto normal2 = glm::normalize(glm::cross(normal1, reflect_dir));

                auto p = random_point({}, {normal1, normal2, {}}, {rough * 0.5f, rough * 0.5f, 0.f});

                reflect_dir = glm::normalize(reflect_dir + p);
                auto reflect_ray = rtr::physics::ray(ctx.hit.position + ctx.hit.normal * 0.001f, reflect_dir);
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
                    regular += res * reflectance;
                }
            }

            return regular;
        }
    }
}

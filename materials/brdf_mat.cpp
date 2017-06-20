//
// Created by fatih on 5/13/17.
//

#include "brdf_mat.hpp"
#include <scene.hpp>
#include <lights.hpp>
#include <materials/shading_ctx.hpp>
#include <physics/ray.hpp>

#include <brdf/phong_brdf.hpp>
#include <brdf/blinn_phong_brdf.hpp>
#include <brdf/torrance_sparrow.hpp>

#include <utility.hpp>
#include <iostream>

namespace rtr
{
namespace shading
{

    template <class BrdfT>
    glm::vec3 brdf_mat<BrdfT>::shade(const shading_ctx& ctx) const
    {
        auto& scene = ctx.scn;

        glm::vec3 ambient = scene.get_ambient().intensity_at(ctx.hit.position) * data.ambient;

        glm::vec3 output = {0, 0, 0};

        auto per_light = [&](const auto& point_to_light, const auto& light_intensity)
        {
            auto len = glm::length(point_to_light);
            auto normalized_ptol = point_to_light / len;

            auto shadow_ray = physics::ray(ctx.hit.position + shadow_epsilon * normalized_ptol, normalized_ptol);
            if (scene.ray_cast_param(shadow_ray, -shadow_epsilon, len)) { return; }

            brdf::brdf_ctx b_ctx {ctx, normalized_ptol};
            auto brdf_res = brdf.calculate(&data, &b_ctx);
            output += brdf_res * light_intensity;
        };

        auto light_handler = make_lambda_visitor<void>(
            [&](const auto* pl)
            {
                auto point_to_light = pl->get_position() - ctx.hit.position;
                per_light(point_to_light, pl->intensity_at(ctx.hit.position));
            },
            [&](const lights::area_light* al)
            {
                auto point_to_light = al->get_position(ctx.hit.r.ms_id) - ctx.hit.position;
                per_light(point_to_light, al->intensity_at(ctx.hit.r.ms_id, ctx.hit.position));
            },
            [&](const lights::directional_light* dl)
            {
                auto point_to_light = dl->get_inverse_dir();
                per_light(point_to_light, dl->intensity_at(ctx.hit.position));
            }
        );

        if (ctx.hit.r.rtl)
        {
            auto ray_dir = rtr::sample_hemisphere(ctx.hit.normal, ctx.hit.r.ms_id, max_ms);

            auto mc_ray = rtr::physics::ray(ctx.hit.position + ray_dir * shadow_epsilon, ray_dir);
            mc_ray.ms_id = ctx.hit.r.ms_id;
            mc_ray.rtl = ctx.hit.r.rtl - 1;

            auto hit_res = ctx.scn.ray_cast(mc_ray);
            if (hit_res)
            {
                auto mc_ctx = shading_ctx{
                    ctx.scn,
                    -ray_dir,
                    *hit_res
                };

                auto monte_carlo_light = hit_res->mat->shade(mc_ctx);

                brdf::brdf_ctx b_ctx {ctx, ray_dir};
                auto brdf_res = brdf.calculate(&data, &b_ctx);
                output += brdf_res * monte_carlo_light;
            }
        }

        scene.for_lights(light_handler);

        return output;
    }

    template class brdf_mat<brdf::phong>;
    template class brdf_mat<brdf::phong_modified>;
    template class brdf_mat<brdf::phong_modified_normalized>;

    template class brdf_mat<brdf::blinn_phong>;
    template class brdf_mat<brdf::blinn_phong_modified>;
    template class brdf_mat<brdf::blinn_phong_modified_normalized>;

    template class brdf_mat<brdf::torrance_sparrow>;
}
}
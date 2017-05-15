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

namespace rtr
{
namespace shading
{
    static constexpr float shadow_epsilon = 0.01f;

    template <class BrdfT>
    glm::vec3 brdf_mat<BrdfT>::shade(const shading_ctx& ctx) const
    {
        auto& scene = ctx.scn;

        glm::vec3 ambient = scene.get_ambient().intensity_at(ctx.hit.position) * data.ambient;

        glm::vec3 output = {};

        auto per_light = [&](const auto& point_to_light, const auto& light_intensity)
        {
            auto len = glm::length(point_to_light);
            auto normalized_ptol = point_to_light / len;
            auto& normal = ctx.hit.normal;

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

        scene.for_lights(light_handler);

        return ambient + output;
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
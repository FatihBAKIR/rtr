//
// Created by fatih on 09.03.2017.
//

#include <glm/vec3.hpp>
#include <materials/rt_mat.hpp>
#include <scene.hpp>
#include <lights.hpp>
#include <utility.hpp>
#include <physics/ray.hpp>
#include <materials/shading_ctx.hpp>

namespace rtr
{
    glm::vec3 rt_mat::shade(const shading_ctx& ctx) const {
        auto& scene = ctx.scn;

        glm::vec3 ambient = scene.get_ambient().intensity_at(ctx.hit.position) * this->ambient;
        glm::vec3 diffuse = {0,0,0};
        glm::vec3 specular = {0,0,0};

        auto light_handler = make_lambda_visitor<void>(
            [&](const lights::point_light* pl)
            {
                auto& normal = ctx.hit.normal;
                auto point_to_light = pl->get_position() - ctx.hit.position;
                auto len = glm::length(point_to_light);
                auto normalized_ptol = point_to_light / len;

                auto shadow_ray = physics::ray(ctx.hit.position + 0.001f * normalized_ptol, normalized_ptol);
                auto res = scene.ray_cast_param(shadow_ray, -0.001f, len);
                if (res) return;

                auto light_intensity = pl->intensity_at(ctx.hit.position);
                auto diffuse_coeff = std::max(0.0f, glm::dot(normal, normalized_ptol));

                auto half_light_view = glm::normalize(ctx.view_dir + normalized_ptol);
                auto specular_coeff = std::pow(std::max(0.0f, glm::dot(normal, half_light_view)), phong);

                diffuse += light_intensity * diffuse_coeff;
                specular += light_intensity * specular_coeff;
            }
        );

        scene.for_lights(light_handler);

        diffuse *= this->diffuse;
        specular *= this->specular;

        return ambient + diffuse + specular;
    }
}

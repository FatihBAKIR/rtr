//
// Created by fatih on 09.03.2017.
//

#include <glm/vec3.hpp>
#include <materials/toon_shading.hpp>
#include <scene.hpp>
#include <lights.hpp>
#include <utility.hpp>
#include <physics/ray.hpp>
#include <materials/shading_ctx.hpp>

namespace rtr
{
    namespace shading
    {
        glm::vec3 toon_shader::shade(const shading_ctx& ctx) const {
            auto& scene = ctx.scn;

            glm::vec3 ambient = scene.get_ambient().intensity_at(ctx.hit.position); //* this->ambient;
            glm::vec3 diffuse = {0,0,0};
            glm::vec3 specular = {0,0,0};

            /*auto light_handler = make_lambda_visitor<void>(
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

                        auto quantize = [](float diffuse_coeff)
                        {
                            if (diffuse_coeff >= 0.98)
                            {
                                diffuse_coeff = 1;
                            }
                            else if (diffuse_coeff >= 0.75)
                            {
                                diffuse_coeff = 0.75;
                            }
                            else if (diffuse_coeff >= 0.5)
                            {
                                diffuse_coeff = 0.5;
                            }
                            else if (diffuse_coeff >= 0.25)
                            {
                                diffuse_coeff = 0.25;
                            }
                            else
                            {
                                diffuse_coeff = 0;
                            }
                            return diffuse_coeff;
                        };

                        diffuse_coeff = quantize(diffuse_coeff);

                        auto half_light_view = glm::normalize(ctx.view_dir + normalized_ptol);

                        auto angle = std::max(0.0f, glm::dot(normal, half_light_view));
                        angle = quantize(angle);
                        auto specular_coeff = std::pow(angle, phong);

                        diffuse += light_intensity * diffuse_coeff;
                        specular += light_intensity * specular_coeff;
                    }
            );

            scene.for_lights(light_handler);*/

            //diffuse *= this->diffuse;
            //specular *= this->specular;

            return ambient + diffuse + specular;
        }
    }
}

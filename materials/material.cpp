//
// Created by fatih on 09.03.2017.
//

#include <glm/vec3.hpp>
#include <materials/material.hpp>
#include <scene.hpp>
#include <lights.hpp>
#include <utility.hpp>
#include <physics/ray.hpp>

namespace rtr
{
    glm::vec3 material::calculate_color(const scene& scene, const glm::vec3& view_dir, const glm::vec3& pos, const glm::vec3 &normal) const {
        glm::vec3 ambient = scene.get_ambient().intensity_at(pos) * this->ambient;
        glm::vec3 diffuse = {0,0,0};
        glm::vec3 specular = {0,0,0};

        auto light_handler = make_lambda_visitor<void>(
            [&](const lights::point_light* pl)
            {
                auto light_intensity = pl->intensity_at(pos);
                auto point_to_light = pl->get_position() - pos;
                auto normalized_ptol = glm::normalize(point_to_light);

                auto shadow_ray = physics::ray(pos + normalized_ptol * 0.001f, normalized_ptol);
                auto res = scene.ray_cast(shadow_ray);

                if (res) return;

                auto diffuse_coeff = std::max(0.0f, glm::dot(normal, point_to_light));

                auto half_light_view = glm::normalize(view_dir + point_to_light);
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

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
#include <texturing/sampler.hpp>
#include <texturing/tex2d.hpp>
#include <texturing/perlin2d.hpp>

namespace rtr
{
    glm::vec3 rt_mat::shade(const shading_ctx& ctx) const {
        auto& scene = ctx.scn;

        glm::vec3 ambient = scene.get_ambient().intensity_at(ctx.hit.position) * this->ambient;
        glm::vec3 diffuse = {0,0,0};
        glm::vec3 specular = {0,0,0};

        auto light_handler = make_lambda_visitor<void>(
            [&](const auto* pl)
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
            },
            [&](const lights::area_light* al)
            {
                auto& normal = ctx.hit.normal;
                auto point_to_light = al->get_position(ctx.hit.r.ms_id) - ctx.hit.position;
                auto len = glm::length(point_to_light);
                auto normalized_ptol = point_to_light / len;

                auto shadow_ray = physics::ray(ctx.hit.position + 0.001f * normalized_ptol, normalized_ptol);
                auto res = scene.ray_cast_param(shadow_ray, -0.001f, len);
                if (res) return;

                auto light_intensity = al->intensity_at(ctx.hit.r.ms_id, ctx.hit.position);
                auto diffuse_coeff = std::max(0.0f, glm::dot(normal, normalized_ptol));

                auto half_light_view = glm::normalize(ctx.view_dir + normalized_ptol);
                auto specular_coeff = std::pow(std::max(0.0f, glm::dot(normal, half_light_view)), phong);

                diffuse += light_intensity * diffuse_coeff;
                specular += light_intensity * specular_coeff;
            }
        );

        auto tex_pos = is_perlin ? ctx.hit.position : glm::vec3(ctx.hit.uv, 0);

        if (mode == decal_mode::replace)
        {
            return this->diffuse_sampler->sample(tex_pos) * 255.f;
        }

        scene.for_lights(light_handler);
        switch (mode)
        {
        case decal_mode::replace:
            break;
        case decal_mode::coeff:
            diffuse *= this->diffuse_sampler->sample(tex_pos);
            break;
        case decal_mode::blend:
            diffuse *= (this->diffuse_sampler->sample(tex_pos) + this->diffuse) * 0.5f;
            break;
        }

        specular *= this->specular;

        return ambient + diffuse + specular;
    }

    rt_mat::rt_mat(const texturing::sampler2d* diff_sampler, decal_mode m, const glm::vec3& specular, const glm::vec3& ambient,
            float phong)
    {
        this->diffuse_sampler = diff_sampler;
        this->mode = m;
        this->specular = specular;
        this->ambient = ambient;
        if (m == decal_mode::replace)
        {
            this->specular = {};
            this->ambient = {};
        }
        this->phong = phong;
        this->is_perlin = dynamic_cast<const texturing::perlin2d*>(diff_sampler)!=nullptr;
    }

    rt_mat::rt_mat(const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& ambient, float phong)
    {
        float test[] = {
            diffuse.r,
            diffuse.g,
            diffuse.b
        };

        auto diffuse_sampler = new texturing::tex2d<float, 3>(test, 1, 1);
        diffuse_sampler->set_sampling_mode(texturing::sampling_mode::nearest_neighbour);
        this->diffuse_sampler = diffuse_sampler;
        this->mode = decal_mode::coeff;

        this->specular = specular;
        this->ambient = ambient;
        this->phong = phong;
        this->is_perlin = false;
    }

    rt_mat::rt_mat(const texturing::sampler2d* diff_sampler, const glm::vec3& diffuse, const glm::vec3& specular,
            const glm::vec3& ambient, float phong)
    {
        this->diffuse_sampler = diff_sampler;
        this->mode = decal_mode::blend;
        this->specular = specular;
        this->ambient = ambient;
        this->diffuse = diffuse;
        this->phong = phong;
        this->is_perlin = dynamic_cast<const texturing::perlin2d*>(diff_sampler)!=nullptr;
    }
}

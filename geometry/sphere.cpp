//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <physics/ray.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "sphere.hpp"
#include <iostream>
#include <utility.hpp>
#include <physics/aabb.hpp>
#include <boost/math/constants/constants.hpp>

constexpr bool SphereInsideCollision = true;

namespace rtr
{
namespace geometry
{
    boost::optional<sphere::param_res_t> sphere::get_parameter(const physics::ray& org_ray) const
    {
        auto ray = physics::ray(glm::vec3(inv_transform * glm::vec4(org_ray.origin, 1)),
                glm::vec3(glm::normalize(glm::vec3(inv_transform * glm::vec4(org_ray.dir, 0)))));

        auto line = ray.origin - get_center();

        float B = 2 * (glm::dot(ray.dir, line));
        float C = glm::length2(line) - r2;
        float minus_B = -B;

        float delta = B * B - 4 * C;

        if (delta < 0)
        {
            return {};
        }

        float root = minus_B - std::sqrt(delta);

        if (root < 0)
        {
            if (!SphereInsideCollision)
            {
                return {};
            }
            root = minus_B + std::sqrt(delta);

            if (root < 0)
            {
                return {};
            }
        }

        auto parameter = root * 0.5f;
        auto pos = ray.origin + ray.dir * parameter;

        auto normal = glm::normalize(glm::vec3(glm::transpose(inv_transform) * glm::vec4(pos - get_center(), 0)));

        glm::vec3 world_pos = glm::vec3(transform * glm::vec4(pos, 1));

        constexpr auto pi = boost::math::constants::pi<float>();

        pos -= this->get_center();

        auto phi = std::atan2(pos.z, pos.x);
        auto theta = std::acos(pos.y / this->get_radius());
        glm::vec2 uv = { phi / (2 * pi), theta / pi };

        auto dpdu = glm::vec3{-pos.z * 2 * pi, 0, pos.x * 2 * pi};
        auto dpdv = glm::vec3{pos.y * pi * std::cos(phi), - get_radius() * pi * std::sin(theta), pos.y * pi * std::sin(phi) };
        dpdu = glm::vec3((transform) * glm::vec4(dpdu, 0));
        dpdv = glm::vec3((transform / glm::determinant(transform)) * glm::vec4(dpdv, 0));

        auto other_normal = glm::normalize(glm::cross(dpdu, dpdv));

        auto diff = glm::dot(other_normal, normal);
        if (diff < 0.99)
        {
            std::cerr << "problem\n";
        }

        return { {parameter, {world_pos, normal, uv, dpdu, dpdv}} };
    }

    physics::ray_hit sphere::intersect(const physics::ray& ray, float parameter, const data_t& data) const
    {
        return physics::ray_hit{ this, ray, mat, data.pos, data.normal, parameter, data.uv, data.dpdu, data.dpdv };
    }

    physics::aabb sphere::bounding_box() const {
        return physics::aabb{get_center(), glm::vec3(get_radius())};
    }
}
}

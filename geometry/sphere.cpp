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

        pos -= this->get_center();
        glm::vec2 uv = { std::atan2(pos.z, pos.x) / (2 * boost::math::constants::pi<float>()),
                         std::acos(pos.y / this->radius) / boost::math::constants::pi<float>() };

        return { {parameter, {world_pos, normal, uv}} };
    }

    physics::ray_hit sphere::intersect(const physics::ray& ray, float parameter, const data_t& data) const
    {
        return physics::ray_hit{ this, ray, mat, data.pos, data.normal, parameter, data.uv };
    }

    physics::aabb sphere::bounding_box() const {
        return physics::aabb{get_center(), glm::vec3(get_radius())};
    }
}
}

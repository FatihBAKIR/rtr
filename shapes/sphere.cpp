//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <physics/ray.hpp>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
#include "sphere.hpp"

constexpr bool SphereInsideCollision = true;

namespace rtr
{
namespace shapes
{
    boost::optional<float> sphere::get_parameter(const physics::ray& ray) const
    {
        auto line = ray.origin - get_center();

        float B = 2 * (glm::dot(ray.dir, line));
        float C = glm::length2(line) - r2;

        float delta = B * B - 4 * C;

        if (delta < 0)
        {
            return {};
        }

        float root = (-B - std::sqrt(delta)) * 0.5f;

        if (root < 0)
        {
            if (!SphereInsideCollision)
            {
                return {};
            }
            root = (-B + std::sqrt(delta)) * 0.5f;

            if (root < 0)
            {
                return {};
            }
        }

        return root;
    }

    physics::ray_hit sphere::intersect(const physics::ray& ray, float parameter) const
    {
        return physics::ray_hit{ ray, mat, {}, {}, parameter };
    }
}
}

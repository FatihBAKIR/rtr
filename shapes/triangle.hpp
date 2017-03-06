//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <rtr_fwd.hpp>
#include <array>
#include <boost/optional.hpp>
#include <glm/vec3.hpp>
#include <gsl/span>
#include <physics/collision.hpp>

namespace rtr
{
namespace shapes
{
    class triangle
    {
        union
        {
            std::array<glm::vec3, 3> vertices;
            struct
            {
                glm::vec3 a;
                glm::vec3 b;
                glm::vec3 c;
            } verts;
        };

    public:

        triangle(const std::array<glm::vec3, 3>& verts) : vertices(verts) {}

        boost::optional<float> get_parameter(const physics::ray& ray) const;
        physics::ray_hit intersect(const physics::ray& ray, float parameter) const;

        gsl::span<const glm::vec3> get_vertices() const
        {
            return vertices;
        }
    };

    physics::collide_result intersect(const physics::aabb&, const triangle&);
}
}

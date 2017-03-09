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
#include <glm/glm.hpp>

namespace rtr
{
namespace shapes
{
    class triangle
    {
        struct tri_corners
        {
            glm::vec3 a;
            glm::vec3 b;
            glm::vec3 c;
        };

        union
        {
            std::array<glm::vec3, 3> vertices;
            tri_corners verts;
        };

        glm::vec3 m_normal;

    public:

        triangle(const std::array<glm::vec3, 3>& vs) : vertices(vs)
        {
            m_normal = glm::normalize(glm::cross(verts.b - verts.a, verts.c - verts.a));
        }

        boost::optional<float> get_parameter(const physics::ray& ray) const;

        gsl::span<const glm::vec3> get_vertices() const
        {
            return vertices;
        }

        glm::vec3 get_normal() const
        {
            return m_normal;
        }
    };

    physics::collide_result intersect(const physics::aabb&, const triangle&);
}
}

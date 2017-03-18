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
#include <transform/transform.hpp>
#include <physics/aabb.hpp>

namespace rtr
{
namespace geometry
{
    inline physics::aabb from_tri(const std::array<glm::vec3, 3>& verts)
    {
         glm::vec3 min = verts[0];
        glm::vec3 max = min;

        for (auto& vert : verts)
        {
            min = glm::min(min, vert);
            max = glm::max(max, vert);
        }

        return physics::from_min_max(min, max);
    }

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

        physics::aabb box;

    public:

        struct param_res_t
        {
            float parameter;
            struct
            {
                float alpha;
                float beta;
                float gamma;
            } data;
        };

        triangle(const std::array<glm::vec3, 3>& vs) : vertices(vs), box(from_tri(vertices))
        {
            m_normal = glm::normalize(glm::cross(verts.b - verts.a, verts.c - verts.a));
        }

        boost::optional<param_res_t> get_parameter(const physics::ray& ray) const;

        gsl::span<const glm::vec3> get_vertices() const
        {
            return vertices;
        }

        glm::vec3 get_normal() const
        {
            return m_normal;
        }

        float get_area() const;

        glm::vec3 get_center() const
        {
            return (verts.a + verts.b + verts.c) * 0.3333333f;
        }

        const physics::aabb& bounding_box() const;
    };

    physics::collide_result intersect(const physics::aabb&, const triangle&);
}
}

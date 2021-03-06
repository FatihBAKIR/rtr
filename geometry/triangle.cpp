//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <geometry/triangle.hpp>
#include <physics/ray.hpp>
#include <physics/aabb.hpp>

namespace rtr {
    namespace geometry {
        float determinant(const glm::vec3& c1, const glm::vec3& c2, const glm::vec3& c3)
        {
            return c1.x * (c2.y * c3.z - c2.z * c3.y) -
                    c2.x * (c1.y * c3.z - c1.z * c3.y) +
                    c3.x * (c1.y * c2.z - c1.z * c2.y);
        }

        physics::collide_result intersect(const rtr::physics::aabb& a, const rtr::geometry::triangle& t)
        {
            return physics::intersect(a, t.get_vertices());
        }

        boost::optional<triangle::param_res_t> triangle::get_parameter(const physics::ray& ray) const
        {
            if (glm::dot(ray.dir, m_normal) > 0)
            {
                return {};
            }

            auto a_c1 = verts.a - verts.b;
            auto a_c2 = verts.a - verts.c;
            auto& a_c3 = ray.dir;
            auto b = verts.a - ray.origin;

            auto detA = determinant(a_c1, a_c2, a_c3);
            auto one_over = 1 / detA;

            auto beta = determinant(b, a_c2, a_c3) * one_over;
            auto gamma = determinant(a_c1, b, a_c3) * one_over;
            auto param = determinant(a_c1, a_c2, b) * one_over;
            auto alpha = 1 - beta - gamma;

            if (beta < -intersection_epsilon || gamma < -intersection_epsilon || alpha < -intersection_epsilon || param < -intersection_epsilon)
            {
                return {};
            }

            return {param_res_t{param, { alpha, beta, gamma }}};
        }

        float triangle::get_area() const
        {
            return glm::length(glm::cross(verts.b - verts.a, verts.c - verts.a)) * 0.5f;
        }

        physics::aabb triangle::bounding_box() const {
            return from_tri(vertices);
        }
    }
}


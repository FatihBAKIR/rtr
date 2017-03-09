//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <shapes/triangle.hpp>
#include <physics/ray.hpp>

namespace rtr {
    namespace shapes {
        float determinant(const glm::vec3& c1, const glm::vec3& c2, const glm::vec3& c3)
        {
            return c1.x * (c2.y * c3.z - c2.z * c3.y) -
                    c2.x * (c1.y * c3.z - c1.z * c3.y) +
                    c3.x * (c1.y * c2.z - c1.z * c2.y);
        }

        physics::collide_result intersect(const rtr::physics::aabb& a, const rtr::shapes::triangle& t)
        {
            return physics::intersect(a, t.get_vertices());
        }

        boost::optional<float> triangle::get_parameter(const physics::ray& ray) const
        {
            auto a_c1 = verts.a - verts.b;
            auto a_c2 = verts.a - verts.c;
            auto& a_c3 = ray.dir;
            auto b = verts.a - ray.origin;

            auto detA = determinant(a_c1, a_c2, a_c3);
            auto beta = determinant(b, a_c2, a_c3) / detA;
            auto gamma = determinant(a_c1, b, a_c3) / detA;
            auto param = determinant(a_c1, a_c2, b) / detA;

            if (beta < -0.0001 || gamma < -0.0001 || (1 - beta - gamma) < -0.0001 || param < -0.0001)
            {
                return {};
            }

            return param;
        }
    }
}


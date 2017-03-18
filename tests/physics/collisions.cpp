//
// Created by fatih on 09.03.2017.
//

#include <shapes/triangle.hpp>
#include <physics/aabb.hpp>

void tri_aabb_collision()
{
    rtr::geometry::triangle tri (std::array<glm::vec3, 3>{ glm::vec3{-0.0888, 0.0877, 0.0142} , glm::vec3{ -0.0896, 0.0915, 0.0178 } , glm::vec3{ -0.0893, 0.0895, 0.0116}  });

    rtr::physics::aabb aabb{ glm::vec3{-0.016800, 0.110152, -0.00148300} , glm::vec3{0.155159, 0.153686, 0.120393}  };

    using rtr::physics::intersect;

    auto res = intersect(aabb, tri);
    assert(res);
}

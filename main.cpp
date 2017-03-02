#include <iostream>

#include <physics/ray.hpp>
#include <shapes/sphere.hpp>
#include <physics/collision.hpp>
#include <physics/aabb.hpp>
#include <physics/octree.hpp>
#include <glm/glm.hpp>


int main()
{
    namespace shapes = rtr::shapes;
    namespace phys = rtr::physics;

    phys::ray r { {1, 1, 1}, glm::normalize(glm::vec3{ 1, 1, 1 }) };

    shapes::sphere s { {0, 0, 0}, 3 };
    shapes::sphere s1 { { -2, -2, -2 }, 1};

    phys::aabb a { {0, 0, 0}, {1, 1, 1} };
    phys::aabb b { {2, 2, 2}, {1, 1, 1} };

    std::cout << phys::intersect(a, r) << '\n';
    std::cout << phys::intersect(b, r) << '\n';

    std::cout << phys::intersect(a, s) << '\n';
    std::cout << phys::intersect(a, a) << '\n';
    std::cout << phys::intersect(a, b) << '\n';
    std::cout << phys::intersect(b, s) << '\n';

    phys::octree oc { { 0, 0, 0}, { 8, 8, 8} };
    oc.add_level();

    if (phys::intersect(oc.bounding_box(), r))
    {
        std::cout << "yay\n";
        for (auto& c : oc.get_children())
        {
            if (phys::intersect(c.bounding_box(), r))
            {
                std::cout << c.bounding_box() << '\n';
                std::cout << "yayay\n";
            }
        }
    }

    return 0;
}
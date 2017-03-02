#include <iostream>

#include <physics/ray.hpp>
#include <shapes/sphere.hpp>
#include <physics/collision.hpp>
#include <physics/aabb.hpp>


int main()
{
    namespace shapes = rtr::shapes;
    namespace phys = rtr::physics;

    phys::ray r { {0, 0, 0}, { 1, 1, 1 } };

    shapes::sphere s { {0, 0, 0}, 3 };

    rtr::aabb a { {0, 0, 0}, {1, 1, 1} };
    rtr::aabb b { {2, 2, 2}, {1, 1, 1} };

    std::cout << rtr::intersect(a, r) << '\n';
    std::cout << rtr::intersect(b, r) << '\n';

    std::cout << rtr::intersect(a, s) << '\n';
    std::cout << rtr::intersect(a, a) << '\n';
    std::cout << rtr::intersect(a, b) << '\n';
    std::cout << rtr::intersect(b, s) << '\n';

    std::cout << "Hello, World!" << std::endl;
    return 0;
}
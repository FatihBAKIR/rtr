#include <iostream>

#include <shapes/sphere.hpp>
#include <shapes/triangle.hpp>
#include <shapes/mesh.hpp>

#include <physics/ray.hpp>

#include <glm/glm.hpp>
#include <vector>

#include <scene.hpp>

static std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
    return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

int main()
{
    namespace shapes = rtr::shapes;
    namespace phys = rtr::physics;
    using phys::intersect;

    phys::ray r { {5, 5, 5}, glm::normalize(glm::vec3{ -1, -1, -1 }) };

    shapes::sphere s { {0, 0, 0}, 3 };
    shapes::sphere s1 { { -2, -2, -2 }, 1};
    shapes::sphere s2 { { 2, 2, 2 }, 1 };

    shapes::triangle t {{ glm::vec3{5, 5, 5}, {6, 5, 5}, {5, 6, 5} }};
    shapes::mesh m ({ t });

    rtr::scene scene;
    scene.insert(s);
    scene.insert(s1);
    scene.insert(m);

    auto res = *scene.ray_cast(r);
}
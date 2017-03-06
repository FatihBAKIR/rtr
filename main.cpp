#include <iostream>

#include <shapes/sphere.hpp>
#include <shapes/triangle.hpp>
#include <shapes/mesh.hpp>

#include <physics/ray.hpp>

#include <glm/glm.hpp>
#include <vector>

#include <scene.hpp>
#include <camera.hpp>

#include <material.hpp>

#define int_p_NULL nullptr

#include <boost/gil/extension/io/png_io.hpp>

#include <xml_parse.hpp>

static std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
    return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

int main()
{

    read_scene("C:/Users/Fatih/Downloads/795_input_set_01/simple.xml");
    return 0;

    namespace shapes = rtr::shapes;
    namespace phys = rtr::physics;

    rtr::material white { { 1.f, 1.f, 1.f } };
    rtr::material pink { { 1.f, .5f, .5f } };

    shapes::sphere s { {0, 5, 0}, 5, &white };
    shapes::sphere s1 { { -2, -2, -2 }, 1, &white};

    shapes::triangle t {{ glm::vec3{100, 0, -100}, {-100, 0, 100}, {100, 0, 100} }};
    shapes::triangle t1 {{ glm::vec3{-100, 0, 100}, {100, 0, -100}, {-100, 0, -100} }};
    shapes::mesh m ({ t, t1 }, &pink);

    rtr::scene scene ({}, {64, 64, 64});
    scene.insert(s);
    scene.insert(s1);
    scene.insert(m);

    scene.finalize();

    rtr::im_plane im_p { -1, 1, 1, -1, 1, 800, 800 };

    rtr::camera cam ({0, 5, 25}, {0, 1, 0}, {0, 0, -1}, im_p);

    auto im = cam.render(scene);

    boost::gil::png_write_view("hai.png", boost::gil::view(im));
}
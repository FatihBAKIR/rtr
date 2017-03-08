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

int main()
{
    auto r = read_scene("/Users/fatih/Downloads/795_input_set_01/bunny.xml");
    r.first.finalize();
    auto ima = r.second[0].render(r.first);
    boost::gil::png_write_view("hai.png", boost::gil::view(ima));
}
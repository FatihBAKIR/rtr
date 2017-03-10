#include <iostream>

#include <lights.hpp>
#include <shapes.hpp>

#include <physics/ray.hpp>

#include <glm/glm.hpp>
#include <vector>

#include <scene.hpp>
#include <camera.hpp>

#include <materials/material.hpp>

#include <xml_parse.hpp>
#include "rtr_config.hpp"

#if RTR_OPENEXR_SUPPORT
    #include <gil_extension/exr/exr_io.hpp>
#else
    #define int_p_NULL nullptr
    #include <boost/gil/extension/io/png_io.hpp>
#endif

int main()
{
    auto r = rtr::xml::read_scene("/Users/fatih/Downloads/795_input_set_01/bunny.xml");
    r.first.finalize();
    auto ima = r.second[0].render(r.first);
    //boost::gil::png_write_view("hai.png", boost::gil::view(ima));
    boost::gil::exr_write_view("hai.exr", boost::gil::view(ima));
}
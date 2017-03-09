#include <iostream>

#include <lights.hpp>
#include <shapes.hpp>

#include <physics/ray.hpp>

#include <glm/glm.hpp>
#include <vector>

#include <scene.hpp>
#include <camera.hpp>

#include <materials/material.hpp>

#define int_p_NULL nullptr

#include <boost/gil/extension/io/png_io.hpp>

#include <xml_parse.hpp>
#include "rtr_config.hpp"

#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImfCompression.h>
#include <OpenEXR/ImfArray.h>

#include <gil_extension/exr/exr_io.hpp>

int main()
{
    auto r = read_scene("/home/fatih/Downloads/795_input_set_01/bunny.xml");
    r.first.finalize();
    auto ima = r.second[0].render(r.first);
    //boost::gil::png_write_view("hai.png", boost::gil::view(ima));
    boost::gil::exr_write_view("hai.exr", boost::gil::view(ima));
}
//
// Created by Mehmet Fatih BAKIR on 05/03/2017.
//

#ifndef RAY_XML_PARSE_HPP
#define RAY_XML_PARSE_HPP

#include <tinyxml2.h>
#include <rtr_fwd.hpp>
#include <iosfwd>

rtr::camera read_camera(const tinyxml2::XMLElement* elem);
void read_scene(const std::string& path);

#endif //RAY_XML_PARSE_HPP
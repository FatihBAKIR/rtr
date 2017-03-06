//
// Created by Mehmet Fatih BAKIR on 05/03/2017.
//

#ifndef RAY_XML_PARSE_HPP
#define RAY_XML_PARSE_HPP

#include <tinyxml2.h>
#include <rtr_fwd.hpp>
#include <iosfwd>
#include <vector>

rtr::camera read_camera(const tinyxml2::XMLElement* elem);
std::pair<rtr::scene, std::vector<rtr::camera>> read_scene(const std::string& path);

#endif //RAY_XML_PARSE_HPP

//
// Created by Mehmet Fatih BAKIR on 05/03/2017.
//

#pragma once

#include <tinyxml2.h>
#include <rtr_fwd.hpp>
#include <iosfwd>
#include <vector>

namespace rtr
{
    namespace xml
    {
        std::pair<rtr::scene, std::vector<rtr::camera>> read_scene(const std::string& path);
    }
}


//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//


#pragma once

#include <boost/optional.hpp>
#include "physics/ray.hpp"

namespace rtr
{
class scene
{
    boost::optional<ray_hit> ray_cast(const ray& ray) const;
};
}
//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <rtr_fwd.hpp>
#include <array>
#include <boost/optional.hpp>

namespace rtr
{
namespace shapes
{
class triangle
{
    std::array<vertex*, 3> vertices;
    struct material* material;

public:
    boost::optional<float> get_parameter(const physics::ray& ray) const;
    physics::ray_hit intersect(const physics::ray& ray, float parameter) const;
};
}
}

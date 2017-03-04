//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include "triangle.hpp"

namespace rtr
{
namespace shapes
{
    physics::collide_result intersect(const rtr::physics::aabb& a, const rtr::shapes::triangle& t)
    {
        return physics::intersect(a, t.get_vertices());
    }

    boost::optional<float> triangle::get_parameter(const physics::ray& ray) const
    {
        return boost::optional<float>();
    }
}
}


//
// Created by fatih on 02.03.2017.
//

#pragma once

#include <physics/aabb.hpp>
#include <boost/container/vector.hpp>
#include <shapes/triangle.hpp>

namespace rtr
{
namespace shapes
{
class mesh {
    physics::aabb bounding;
    boost::container::vector<triangle> tris;


public:

    const physics::aabb& bounding_box() const
    {
        return bounding;
    }
};
}
}


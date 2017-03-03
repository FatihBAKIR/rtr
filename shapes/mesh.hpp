//
// Created by fatih on 02.03.2017.
//

#pragma once

#include <boost/container/vector.hpp>
#include <shapes/triangle.hpp>
#include <physics/octree.hpp>
#include <gsl/gsl>

namespace rtr
{
namespace shapes
{
class mesh {
    physics::octree part;

public:

    mesh(const gsl::span<vertex>& vertices);

    const physics::aabb& bounding_box() const
    {
        return part.bounding_box();
    }
};
}
}


//
// Created by fatih on 02.03.2017.
//

#pragma once

#include <boost/container/container_fwd.hpp>
#include <shapes/triangle.hpp>
#include <physics/octree.hpp>
#include <gsl/gsl>

namespace rtr
{
namespace shapes
{
class mesh {
    boost::container::vector<triangle> tris;
    physics::octree<triangle> part;

public:

    mesh(boost::container::vector<triangle> tris);
    ~mesh();

    boost::optional<float> get_parameter(const physics::ray& ray) const;
    physics::ray_hit intersect(const physics::ray& ray, float parameter) const;

    const physics::aabb& bounding_box() const
    {
        return part.bounding_box();
    }
};
}
}


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

    using octree_type = physics::octree<triangle>;

    boost::container::vector<triangle> tris;
    octree_type part;

    const material* mat;

public:

    mesh(boost::container::vector<triangle> tris, const material* mat);
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


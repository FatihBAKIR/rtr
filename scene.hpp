//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//


#pragma once

#include <rtr_fwd.hpp>
#include <boost/optional.hpp>
#include <physics/octree.hpp>

namespace rtr
{
class scene
{
    physics::octree part;
    bvector<vertex> vertices;

    bvector<shapes::mesh> meshes;
    bvector<shapes::sphere> spheres;

public:

    scene();
    ~scene();

    boost::optional<physics::ray_hit> ray_cast(const physics::ray& ray) const;

    template <class T>
    void insert(T&& obj)
    {

    }
};
}
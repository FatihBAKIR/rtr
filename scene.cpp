//
// Created by fatih on 02.03.2017.
//

#include <scene.hpp>
#include <physics/ray.hpp>
#include <shapes/mesh.hpp>
#include <shapes/sphere.hpp>
#include <vertex.hpp>

boost::optional<rtr::physics::ray_hit> rtr::scene::ray_cast(const rtr::physics::ray &ray) const {
    return {};
}

rtr::scene::scene() : part({}, {})
{

}

rtr::scene::~scene() = default;
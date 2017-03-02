//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <rtr_fwd.hpp>
#include <glm/vec3.hpp>
#include <physics/collision.hpp>
#include <physics/aabb.hpp>

namespace rtr
{
class octree
{
    aabb box;

public:
    void insert(const shapes::sphere& sphere);
    void insert(const shapes::triangle& tri);

};
}

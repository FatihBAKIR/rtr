//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <rtr_fwd.hpp>

namespace rtr
{
namespace physics
{
    using collide_result = bool;

    collide_result intersect(const aabb&, const shapes::sphere&);
    collide_result intersect(const aabb&, const aabb&);
    collide_result intersect(const aabb&, const shapes::triangle&);
    collide_result intersect(const aabb&, const ray&);
}
}
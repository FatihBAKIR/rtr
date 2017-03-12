//
// Created by fatih on 12.03.2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <rtr_fwd.hpp>

namespace rtr
{
    struct shading_ctx
    {
        const scene& scn;
        glm::vec3 view_dir;
        const rtr::physics::ray_hit& hit;
    };
}

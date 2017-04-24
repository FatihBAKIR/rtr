//
// Created by fatih on 4/24/17.
//

#pragma once

#include <glm/vec3.hpp>

namespace rtr
{
namespace lights
{
    class directional_light
    {
        glm::vec3 dir;
        glm::vec3 radiance;
    public:

        directional_light(glm::vec3 d, glm::vec3 r) :
            dir(d), radiance(r)
        {
        }

        glm::vec3 get_inverse_dir() const
        {
            return -dir;
        }

        glm::vec3 intensity_at(const glm::vec3& /*at*/) const
        {
            return radiance;
        }
    };
}
}

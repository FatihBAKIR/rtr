//
// Created by fatih on 09.03.2017.
//

#pragma once

#include <glm/vec3.hpp>

namespace rtr
{
    namespace lights
    {
        class ambient_light {
            glm::vec3 intensity;
        public:
            ambient_light(const glm::vec3& intensity) : intensity(intensity){}

            glm::vec3 intensity_at(const glm::vec3& at) const
            {
                return intensity;
            }
        };
    }
}

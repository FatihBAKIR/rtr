//
// Created by Mehmet Fatih BAKIR on 09/03/2017.
//

#pragma once

#include <glm/gtx/norm.hpp>

namespace rtr {
    namespace lights {
        class point_light
        {
            glm::vec3 position;
            glm::vec3 intensity;

        public:
            point_light(const glm::vec3& pos, const glm::vec3& intensity) :
                    position(pos), intensity(intensity)
            { }

            glm::vec3 get_position() const
            {
                return position;
            }

            glm::vec3 intensity_at(const glm::vec3& at) const
            {
                return intensity / (glm::length2(at - position));
            }
        };
    }
}

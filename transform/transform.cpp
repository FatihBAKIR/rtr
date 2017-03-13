//
// Created by fatih on 12.03.2017.
//

#include "transform.hpp"
#include <glm/gtx/transform.hpp>

namespace rtr
{
    namespace transform
    {
        glm::mat4 translate(const glm::vec3 & dis) {
            return glm::translate(dis);
        }

        glm::mat4 scale(const glm::vec3 & s) {
            return glm::scale(s);
        }

        glm::mat4 rotate(float degrees, const glm::vec3 &axis) {
            return glm::rotate(glm::radians(degrees), axis);
        }
    }
}


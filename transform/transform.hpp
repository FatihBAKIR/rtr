//
// Created by fatih on 12.03.2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

namespace rtr
{
    namespace transform
    {
        glm::mat4 translate(const glm::vec3&);
        glm::mat4 scale(const glm::vec3&);

        glm::mat4 rotate(float degrees, const glm::vec3& axis);
    }
}


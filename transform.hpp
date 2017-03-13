//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <glm/vec3.hpp>

namespace rtr
{
struct transformation
{
    const glm::vec3 position;
    const glm::vec3 up;
    const glm::vec3 forward;
    const glm::vec3 right;
};
}

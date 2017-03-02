//
// Created by Mehmet Fatih BAKIR on 02/03/2017.
//

#pragma once

#include <glm/vec3.hpp>

namespace rtr
{
struct aabb
{
    const glm::vec3 position;
    const glm::vec3 extent;
    const glm::vec3 min;
    const glm::vec3 max;

    aabb(const glm::vec3& pos, const glm::vec3& ext)
            : position(pos), extent(ext), min(pos - ext * 0.5f), max(pos + ext * 0.5f)
    {
    }
};
}

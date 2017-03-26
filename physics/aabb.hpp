//
// Created by Mehmet Fatih BAKIR on 02/03/2017.
//

#pragma once

#include <glm/glm.hpp>
#include <ostream>
#include <numeric>

namespace rtr
{
namespace physics
{
struct aabb {
    glm::vec3 position;
    glm::vec3 extent;
    glm::vec3 min;
    glm::vec3 max;

    aabb(const glm::vec3 &pos, const glm::vec3 &ext)
            : position(pos), extent(ext), min(pos - ext * 0.5f), max(pos + ext * 0.5f) {
    }

    friend std::ostream& operator<<(std::ostream& os, const aabb& box)
    {
        return os << "{ (" << box.position.x << ", " << box.position.y << ", " << box.position.z << "), "
           << "(" << box.extent.x << ", " << box.extent.y << ", " << box.extent.z << ") }";
    }
};

    inline aabb from_min_max(const glm::vec3& min, const glm::vec3& max)
    {
        return { (min + max) * 0.5f, max - min };
    }

    inline aabb merge(const aabb& a, const aabb& b)
    {
        auto min = glm::min(a.min, b.min);
        auto max = glm::max(a.max, b.max);

        return from_min_max(min, max);
    }

    template <class IteratorT>
    inline aabb merge(IteratorT begin, IteratorT end)
    {
        return std::accumulate(std::next(begin), end, begin->bounding_box(), [](const aabb& prev, const auto& elem)
        {
            return merge(prev, elem.bounding_box());
        });
    }
}
}

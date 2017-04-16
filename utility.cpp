//
// Created by Mehmet Fatih BAKIR on 08/03/2017.
//

#include <utility.hpp>
#include <ostream>
#include <random>

namespace glm
{
    std::ostream& operator<<(std::ostream& os, const vec3& v)
    {
        return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    }
}

namespace rtr
{
    static std::mt19937 rng;
    glm::vec3 random_point(const glm::vec3& around, float x_range, float y_range, float z_range)
    {
        std::uniform_real_distribution<float> x_dist(-x_range, x_range);
        std::uniform_real_distribution<float> y_dist(-y_range, y_range);
        std::uniform_real_distribution<float> z_dist(-z_range, z_range);
        float x_deviate = x_dist(rng);
        float y_deviate = y_dist(rng);
        float z_deviate = z_dist(rng);

        return around + glm::vec3(x_deviate, y_deviate, z_deviate);
    }
}


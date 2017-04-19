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
    glm::vec3 random_point(const glm::vec3& around, const std::array<glm::vec3, 3>& a, const std::array<float, 3>& ranges)
    {
        std::uniform_real_distribution<float> x_dist(-ranges[0], ranges[0]);
        std::uniform_real_distribution<float> y_dist(-ranges[1], ranges[1]);
        std::uniform_real_distribution<float> z_dist(-ranges[2], ranges[2]);
        float x_deviate = x_dist(rng);
        float y_deviate = y_dist(rng);
        float z_deviate = z_dist(rng);

        return around + x_deviate * a[0] + y_deviate * a[1] + z_deviate * a[2];
    }

    glm::vec3 get_normal(const glm::vec3& dir)
    {
        auto res = glm::cross(dir, glm::vec3(1, 0, 0));
        if (glm::length(res) <= 0.001)
        {
            res = glm::cross(dir, glm::vec3(0, 1, 0));
        }
        return res;
    }
}


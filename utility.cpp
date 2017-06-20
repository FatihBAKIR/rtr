//
// Created by Mehmet Fatih BAKIR on 08/03/2017.
//

#include <utility.hpp>
#include <ostream>
#include <random>
#include <glm/glm.hpp>
#include <boost/math/constants/constants.hpp>
#include <cmath>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace glm
{
    std::ostream& operator<<(std::ostream& os, const vec3& v)
    {
        return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    }
}

namespace rtr
{
    static constexpr auto pi = boost::math::constants::pi<float>();
    static std::mt19937 rng(0);
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

    glm::vec3 sample_cosine(float u1, float u2)
    {
        const float r = std::sqrt(u1);
        const float theta = 2 * pi * u2;

        const float x = r * std::cos(theta);
        const float y = r * std::sin(theta);

        return glm::vec3(x, y, std::sqrt(std::max(0.0f, 1 - u1)));
    }

    glm::vec3 sample_hemisphere(int ms_id, int max_ms)
    {
        static std::uniform_real_distribution<float> dist1(0, 1);
        static std::uniform_real_distribution<float> dist2(0, 1);
        auto ksi1 = dist1(rng);
        auto ksi2 = dist2(rng);
        return sample_cosine(ksi1, ksi2);

        /*ksi1 = lerp(float(ms_id) / max_ms, float(ms_id + 1) / max_ms, ksi1);
        ksi2 = lerp(float(ms_id) / max_ms, float(ms_id + 1) / max_ms, ksi2);*/

        auto v = glm::vec3(
            std::cos(2 * pi * ksi2) * std::sqrt(1 - ksi1 * ksi1),
            ksi1,
            std::sin(2 * pi * ksi2) * std::sqrt(1 - ksi2 * ksi2));
        return v;
    }

    glm::vec3 sample_hemisphere(const glm::vec3& towards, int ms_id, int max_ms)
    {
        auto base_sample = sample_hemisphere(ms_id, max_ms);
        if (towards == glm::vec3(0, 0, -1))
        {
            return -base_sample;
        }

        auto axis = glm::cross(glm::vec3(0, 0, 1), towards);
        auto angle = std::acos(glm::dot(towards, glm::vec3(0, 0, 1)));

        auto rotate = glm::angleAxis(angle, axis);
        return rotate * base_sample;
    }
}

//
// Created by Mehmet Fatih BAKIR on 19/04/2017.
//

#pragma once

namespace rtr
{
namespace lights
{
    class spot_light
    {
        glm::vec3 position;
        glm::vec3 intensity;
        glm::vec3 dir;

        float coverage;
        float fall_off;

    public:
        spot_light(const glm::vec3& pos, const glm::vec3& intensity, const glm::vec3& dir, float cover, float falloff) :
                position(pos), intensity(intensity), dir(dir), coverage(glm::radians(cover)), fall_off(glm::radians(falloff))
        { }

        glm::vec3 get_position() const
        {
            return position;
        }

        glm::vec3 intensity_at(const glm::vec3& at) const
        {
            auto p_dir = at - position;
            auto norm_dir = glm::normalize(p_dir);
            auto cos = glm::dot(dir, norm_dir);
            auto radians = std::acos(cos);

            if (radians > coverage)
            {
                return {};
            }

            auto alpha = glm::clamp(0.f, 1.f, (radians - coverage) / (fall_off - coverage));
            auto alpha2 = alpha*alpha;
            auto alpha4 = alpha2*alpha2;

            return intensity * alpha4 / glm::length2(p_dir);
        }
    };
}
}

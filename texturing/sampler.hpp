//
// Created by fatih on 4/21/17.
//

#pragma once

#include <glm/glm.hpp>

namespace rtr
{
namespace texturing
{
    enum class sampling_mode
    {
        nearest_neighbour,
        bilinear
    };

    class sampler2d
    {
    public:
        virtual glm::vec3 sample(const glm::vec3& pos) const = 0;
        glm::vec3 sample(const glm::vec2& uv) const { return sample(glm::vec3(uv, 0)); };
        virtual void set_sampling_mode(sampling_mode) {}
        virtual ~sampler2d() = 0;
    };
    inline sampler2d::~sampler2d() = default;
}
}

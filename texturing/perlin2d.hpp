//
// Created by fatih on 4/23/17.
//

#pragma once

#include <texturing/sampler.hpp>
#include <memory>

namespace rtr
{
namespace texturing
{
    enum class perlin_appearance
    {
        vein,
        patch
    };

    struct perlin2d_impl;
    class perlin2d : public sampler2d
    {
        std::unique_ptr<perlin2d_impl> pimpl;
    public:
        perlin2d(perlin_appearance, float);
        glm::vec3 sample(const glm::vec3& pos) const override;
        void set_sampling_mode(sampling_mode mode) override;
        ~perlin2d() override;
    };
}
}


//
// Created by fatih on 4/19/17.
//

#pragma once

#include <utility.hpp>

namespace rtr
{
namespace lights
{
    class area_light
    {
        glm::vec3 position;
        glm::vec3 intensity;

        glm::vec3 edge_1;
        glm::vec3 edge_2;

        int sample_sqrt = 10;
        float e1_rng, e2_rng;

    public:
        area_light(const glm::vec3& pos, const glm::vec3& inte, const glm::vec3& e1, const glm::vec3& e2, int samples)
        {
            position = pos;
            intensity = inte;
            edge_1 = e1;
            edge_2 = e2;
            sample_sqrt = samples;
            e1_rng = glm::length(edge_1) * 0.5f / sample_sqrt;
            e2_rng = glm::length(edge_2) * 0.5f / sample_sqrt;
        }

        glm::vec3 get_position(int id) const
        {
            float x = id % sample_sqrt;
            float y = id / sample_sqrt;
            auto p = position + x * edge_1 / (float)sample_sqrt + y * edge_2 / (float)sample_sqrt;

            return random_point(p, {edge_1, edge_2, {}}, {e1_rng, e2_rng, 0});
        }

        glm::vec3 intensity_at(int id, const glm::vec3& at) const
        {
            return intensity / (glm::length2(at - get_position(id)));
        }
    };
}
}

//
// Created by fatih on 18.03.2017.
//

#include "toon_shading.hpp"
#include <materials/normal_mat.hpp>
#include <physics/ray.hpp>

glm::vec3 rtr::shading::toon_shader::shade(const rtr::shading_ctx &ctx) const {
    auto new_hit = ctx.hit;
    new_hit.normal = get_closest(ctx.hit.normal);
    auto new_ctx = shading_ctx{ ctx.scn, ctx.view_dir, new_hit };
    return base_mat->shade(new_ctx);
}
constexpr double pi() { return std::atan(1)*4; }
rtr::shading::toon_shader::toon_shader(const material* base, int M, int N) {
    base_mat = base; //take this as a parameter

    //(x, y, z) = (sin(Pi * m/M) cos(2Pi * n/N), sin(Pi * m/M) sin(2Pi * n/N), cos(Pi * m/M))

    for (int m = 0; m < M; ++m) {
        for (int n = 0; n < N - 1; ++n) {
            quantized.push_back(
                    {std::sin(pi() * m / M) * std::cos(2.f * pi() * n / N),
                     std::sin(pi() * m / M) * std::sin(2.f * pi() * n / N),
                     std::cos(pi() * m / M)});
        }
    }
}

const glm::vec3 &rtr::shading::toon_shader::get_closest(const glm::vec3 &normal) const {
    const glm::vec3* closest = nullptr;
    float max_dot = 0;

    for (auto& vec : quantized)
    {
        auto dp = glm::dot(normal, vec);
        if (dp > max_dot)
        {
            max_dot = dp;
            closest = &vec;
        }
    }

    return *closest;
}

//
// Created by fatih on 18.03.2017.
//

#include "toon_shading.hpp"
#include <materials/normal_mat.hpp>
#include <physics/ray.hpp>
#include <boost/math/constants/constants.hpp>

float luminance(const glm::vec3& col)
{
    return 0.2126f * col.r + 0.7152f * col.g + 0.0722f * col.b;
}

glm::vec3 set_luminance(const glm::vec3& col, float new_lumin)
{
    return (col / luminance(col)) * new_lumin;
}

glm::vec3 rtr::shading::toon_shader::shade(const rtr::shading_ctx &ctx) const {
    auto color = base_mat->shade(ctx);

    constexpr float step = 30;

    auto intensity = luminance(color);
    auto updated = std::round(intensity / step) * step;

    return set_luminance(color, updated);
}
constexpr double pi() { return boost::math::constants::pi<double>(); }
rtr::shading::toon_shader::toon_shader(const material* base, int M, int N) {
    base_mat = base; //take this as a parameter
}

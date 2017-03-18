//
// Created by fatih on 12.03.2017.
//

#include <physics/ray.hpp>
#include <materials/normal_mat.hpp>

glm::vec3 rtr::normal_mat::shade(const rtr::shading_ctx &ctx) const {
    return (ctx.hit.normal + glm::vec3(1, 1, 1)) * 0.5f * 255.f;
}

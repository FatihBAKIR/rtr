//
// Created by fatih on 12.03.2017.
//

#include <physics/ray.hpp>
#include <materials/normal_mat.hpp>

glm::vec3 rtr::normal_mat::shade(const rtr::shading_ctx &ctx) const {
    return ctx.hit.normal * 255.f;
}

//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <rtr_fwd.hpp>
#include <transform.hpp>
#include <boost/optional.hpp>

namespace rtr
{
namespace shapes
{
class sphere
{
    glm::vec3 pos;
    float radius;
    float r2;

    const material* mat;

public:

    sphere(const glm::vec3& p, float r, const material* m) : pos(p), radius(r), r2(r * r), mat(m)
    {}

    const glm::vec3& get_center() const { return pos; }
    float get_radius() const { return radius; }

    boost::optional<float> get_parameter(const physics::ray& ray) const;
    physics::ray_hit intersect(const physics::ray& ray, float parameter) const;
};
}
}


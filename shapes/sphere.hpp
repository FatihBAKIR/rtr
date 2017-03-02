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

public:

    sphere(const glm::vec3& p, float r) : pos(p), radius(r)
    {}

    constexpr const glm::vec3& get_center() const { return pos; }
    constexpr float get_radius() const { return radius; }

    constexpr boost::optional<float> get_parameter(const physics::ray& ray) const;
    constexpr physics::ray_hit intersect(const physics::ray& ray, float parameter) const;
};
}
}


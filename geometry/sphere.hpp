//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <rtr_fwd.hpp>
#include <transform.hpp>
#include <boost/optional.hpp>
#include <glm/mat4x4.hpp>

namespace rtr
{
namespace geometry
{
class sphere
{
    glm::vec3 pos;
    float radius;
    float r2;

    const material* mat;

    glm::mat4 inv_transform;
    glm::mat4 transform;
    int m_id;

public:

    struct data_t
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
        glm::vec3 dpdu, dpdv;
    };

    struct param_res_t
    {
        float parameter;
        data_t data;
    };

    sphere(const glm::vec3& p, float r, const material* m, const glm::mat4& trans) : pos(p), radius(r), r2(r * r), mat(m),
                                                                                   inv_transform(glm::inverse(trans)),
                                                                                     transform(trans)
    {}

    const glm::vec3& get_center() const { return pos; }
    float get_radius() const { return radius; }

    boost::optional<param_res_t> get_parameter(const physics::ray& ray) const;
    physics::ray_hit intersect(const physics::ray& ray, float parameter, const data_t& data) const;

    physics::aabb bounding_box() const;

    void set_id(int id)
    {
        m_id = id;
    }
    int get_id() const {
        return m_id;
    }
};
}
}


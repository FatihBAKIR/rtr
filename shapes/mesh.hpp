//
// Created by fatih on 02.03.2017.
//

#pragma once

#include <boost/container/container_fwd.hpp>
#include <shapes/triangle.hpp>
#include <physics/octree.hpp>
#include <gsl/gsl>

namespace rtr
{
namespace shapes
{
class mesh {

    using octree_type = physics::octree<triangle>;

    boost::container::vector<long> face_indices;

    boost::container::vector<triangle> tris;
    boost::container::vector<glm::vec3> vert_normals;
    octree_type part;

    const material* mat;

public:

    struct data_t
    {
        const triangle* tri;
        float alpha;
        float beta;
        float gamma;
    };
    struct param_res_t
    {
        float parameter;
        data_t data;
    };

    mesh(boost::container::vector<triangle> tris, boost::container::vector<long> indices, const material* mat);
    mesh(const mesh&) = delete;
    mesh(mesh&&) noexcept;
    ~mesh() noexcept;

    boost::optional<param_res_t> get_parameter(const physics::ray& ray) const;
    physics::ray_hit intersect(const physics::ray& ray, float parameter, data_t&) const;

    void smooth_normals();

    const physics::aabb& bounding_box() const
    {
        return part.bounding_box();
    }
};
}
}


//
// Created by fatih on 02.03.2017.
//

#pragma once

#include <boost/container/container_fwd.hpp>
#include <geometry/triangle.hpp>
#include <physics/octree.hpp>
#include <gsl/gsl>
#include <physics/bvh.hpp>

namespace rtr
{
namespace geometry
{
class mesh {
    using bvh_type = std::unique_ptr<physics::bvh<triangle>>;

    boost::container::vector<int> face_indices;

    boost::container::vector<triangle> tris;
    boost::container::vector<glm::vec3> vert_normals;

    boost::container::vector<glm::vec2> uvs;

    bvh_type hier;

    const material* mat;
    int m_id = -1;

    bool m_local_scene = false;

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

    mesh(boost::container::vector<triangle> tris, boost::container::vector<int> indices, boost::container::vector<glm::vec2> uv, const material* mat);
    mesh(const mesh&) = delete;
    mesh(mesh&&) noexcept;
    ~mesh() noexcept;

    boost::optional<param_res_t> get_parameter(const physics::ray& ray) const;
    physics::ray_hit intersect(const physics::ray& ray, float parameter, data_t&) const;

    void smooth_normals();

    const physics::aabb& bounding_box() const
    {
        return hier->bounding_box();
    }

    glm::vec3 get_center() const
    {
        return hier->bounding_box().position;
    }


    void set_id(int id)
    {
        m_id = id + 127;
    }
    int get_id() const
    {
        return m_id;
    }

    void set_local_scene()
    {
        m_local_scene = true;
    }
};
}
}


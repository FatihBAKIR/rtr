//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <rtr_fwd.hpp>
#include <glm/vec3.hpp>
#include <physics/aabb.hpp>
#include <boost/container/vector.hpp>
#include <boost/variant.hpp>
#include <boost/container/static_vector.hpp>

namespace rtr
{
namespace physics
{
class octree
{
    aabb box;
    bvector<octree> children;

    bvector<const shapes::mesh*> meshes;
    bvector<const shapes::sphere*> spheres;

    void put(const shapes::sphere* s);
    void put(const shapes::mesh* m);

    template <class ShapeT>
    void insert_impl(const ShapeT& shape);

public:
    octree(const glm::vec3& center, const glm::vec3& extent);

    const aabb& bounding_box() const { return box; }
    const bvector<octree>& get_children() const { return children; }

    template <class FunT>
    void for_shapes(FunT&& f) const
    {
        std::for_each(meshes.begin(), meshes.end(), f);
        std::for_each(spheres.begin(), spheres.end(), f);
    }

    void insert(const shapes::sphere& sphere);
    void insert(const shapes::mesh& mesh);

    void add_level();

    ~octree();
};
}
}

//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//


#pragma once

#include <rtr_fwd.hpp>
#include <boost/optional.hpp>
#include <physics/octree.hpp>
#include <unordered_map>
#include <material.hpp>

namespace rtr
{
class scene
{
    using shape_list = png::list<shapes::sphere, shapes::mesh>;
    using octree_type = png::convert_t<physics::octree, shape_list>;
    using vectors = png::map_t<png::mapper<bvector>, shape_list>;
    using vector_tuple = png::convert_t<boost::fusion::vector, vectors>;

    octree_type part;
    vector_tuple shapes;

    std::unordered_map<long, material> mats;

public:

    scene(const glm::vec3& center, const glm::vec3& extent, std::unordered_map<long, material> mats);
    scene(const scene&) = delete;
    scene(scene&&) = default;
    ~scene();

    boost::optional<physics::ray_hit> ray_cast(const physics::ray& ray) const;

    auto& materials() const { return mats; }

    template <class T>
    void insert(T&& obj)
    {
        constexpr auto index = png::index_of_t<std::decay_t<T>, shape_list>();
        auto& v = boost::fusion::at_c<index>(shapes);
        v.push_back(std::forward<T>(obj));
    }

    void finalize();
};
}
//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//


#pragma once

#include <rtr_fwd.hpp>
#include <boost/optional.hpp>
#include <physics/octree.hpp>

namespace rtr
{
class scene
{
    using shape_list = png::list<shapes::mesh, shapes::sphere>;
    using octree_type = png::convert_t<physics::octree, shape_list>;
    using vectors = png::map_t<png::mapper<bvector>, shape_list>;
    using vector_tuple = png::convert_t<boost::fusion::vector, vectors>;

    octree_type part;
    vector_tuple shapes;

public:

    scene();
    ~scene();

    boost::optional<physics::ray_hit> ray_cast(const physics::ray& ray) const;

    template <class T>
    void insert(T&& obj)
    {
        constexpr auto index = png::index_of_t<std::decay_t<T>, shape_list>();
        auto& v = boost::fusion::at_c<index>(shapes);
        v.push_back(std::forward<T>(obj));
        part.insert(v.back());
    }
};
}
//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//


#pragma once

#include <rtr_fwd.hpp>
#include <boost/optional.hpp>
#include <physics/octree.hpp>
#include <unordered_map>
#include <material.hpp>
#include <lights/ambient_light.hpp>

namespace rtr
{
class scene
{
    using shape_list = png::list<shapes::sphere, shapes::mesh>;
    using shape_vectors = png::map_t<png::mapper<bvector>, shape_list>;
    using shape_vector_tuple = png::convert_t<boost::fusion::vector, shape_vectors>;

    using light_list = png::list<>;
    using light_vectors = png::map_t<png::mapper<bvector>, light_list>;
    using light_vector_tuple = png::convert_t<boost::fusion::vector, light_vectors>;

    using octree_type = png::convert_t<physics::octree, shape_list>;

    octree_type part;
    shape_vector_tuple shapes;
    light_vector_tuple lights;
    lights::ambient_light ambient;

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

    template <class FunT>
    void for_lights(const FunT& fun) const
    {
        boost::fusion::for_each(lights, [&](const auto& vector)
        {
            std::for_each(vector.begin(), vector.end(), [&](auto& light)
            {
                fun(&light);
            });
        });
    }

    const lights::ambient_light& get_ambient() const
    {
        return ambient;
    }

    void finalize();
};
}
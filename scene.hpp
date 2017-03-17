//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//


#pragma once

#include <rtr_fwd.hpp>
#include <boost/optional.hpp>
#include <physics/octree.hpp>
#include <unordered_map>
#include <materials/rt_mat.hpp>

namespace rtr
{
class scene
{
    using shape_list = png::list<shapes::sphere, shapes::mesh>;
    using shape_vectors = png::map_t<png::mapper<bvector>, shape_list>;
    using shape_vector_tuple = png::convert_t<boost::fusion::vector, shape_vectors>;

    using light_list = png::list<lights::point_light>;
    using light_vectors = png::map_t<png::mapper<bvector>, light_list>;
    using light_vector_tuple = png::convert_t<boost::fusion::vector, light_vectors>;

public:

    using octree_type = png::convert_t<physics::octree, shape_list>;
    glm::vec3 m_background;
    float m_shadow_epsilon;
    float m_test_epsilon;

    scene(const glm::vec3& center, const glm::vec3& extent, std::unordered_map<long, material*> mats);
    scene(const scene&) = delete;
    scene(scene&&) = default;
    ~scene();

    boost::optional<physics::ray_hit> ray_cast(const physics::ray& ray) const;
    bool ray_cast_param(const physics::ray& ray, float min_param, float max_param) const;

    auto& materials() const { return mats; }
    const physics::aabb& get_box() const {
        return part.bounding_box();
    }

    void insert(const lights::ambient_light& light);
    void insert(lights::ambient_light&& light);

    template <class LightT>
    std::enable_if_t<png::index_of_t<std::decay_t<LightT>, light_list>() >= 0>
    insert(LightT&& light)
    {
        constexpr auto index = png::index_of_t<std::decay_t<LightT>, light_list>();
        auto& v = boost::fusion::at_c<index>(lights);
        v.push_back(std::forward<LightT>(light));
    }

    template <class T>
    std::enable_if_t<png::index_of_t<std::decay_t<T>, shape_list>() >= 0>
    insert(T&& obj)
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
        Expects(ambient != nullptr);
        return *ambient;
    }

    void resize(const glm::vec3& pos, const glm::vec3& extent);
    void finalize();
private:

    octree_type part;
    shape_vector_tuple shapes;
    light_vector_tuple lights;
    std::unique_ptr<lights::ambient_light> ambient;

    std::unordered_map<long, material*> mats;
};
}
//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#pragma once

#include <rtr_fwd.hpp>
#include <glm/vec3.hpp>
#include <physics/aabb.hpp>
#include <boost/container/vector.hpp>
#include <boost/variant.hpp>
#include <meta_list.hpp>
#include <boost/tuple/tuple.hpp>
#include <physics/collision.hpp>
#include <array>
#include <boost/fusion/algorithm.hpp>
#include <queue>

namespace rtr
{
namespace physics
{
    template <class... Types>
    class octree
    {
        template<class T>
        using bvector = boost::container::vector<T>;

        using types_list = png::list<Types...>;
        using const_types = png::map_t<png::mapper<std::add_const_t>, types_list>;
        using types_p_list = png::map_t<png::mapper<std::add_pointer_t>, const_types>;
        using vectors_list = png::map_t<png::mapper<bvector>, types_p_list>;
        using vector_tuple = png::convert_t<boost::fusion::vector, vectors_list>;

        aabb box;
        bvector<octree> children;

        vector_tuple ptr_vectors;

        unsigned long size;

        template <class ShapeT>
        void put(const ShapeT* p);

        template <class ShapeT>
        void erase(const ShapeT* p);

        template<class ShapeT>
        octree* insert_impl(const ShapeT& shape);

        template <class ShapeT>
        octree* where(const ShapeT& shape);
    public:
        octree(const glm::vec3 &center, const glm::vec3 &extent) : box(center, extent), size{}
        {}

        const aabb& bounding_box() const
        { return box; }

        const bvector<octree>& get_children() const
        { return children; }

        template<class FunT>
        void for_shapes(FunT&& f) const
        {
            boost::fusion::for_each(ptr_vectors, [&](const auto& vector)
            {
                std::for_each(vector.begin(), vector.end(), f);
            });
        }

        template <class T>
        octree* insert(const T& shape)
        {
            static_assert(png::index_of_t<T, types_list>() >= 0, "shape is not in list");
            return insert_impl(shape);
        }

        void resize(const glm::vec3& center, const glm::vec3& extent);
        void add_level();
        unsigned long get_size() { return size; }
    };

    template <class... Ts>
    template<class ShapeT>
    octree<Ts...>* octree<Ts...>::insert_impl(const ShapeT &shape) {
        auto to = where(shape);
        Expects(to != nullptr);
        to->put(&shape);
        return to;
    }

    template <class... Ts>
    template<class T>
    void octree<Ts...>::put(const T* p)
    {
        constexpr auto index = png::index_of_t<T, types_list>();
        auto& v = boost::fusion::at_c<index>(ptr_vectors);
        v.push_back(p);
        ++size;
    }

    template <class... Ts>
    void octree<Ts...>::add_level() {
        children.reserve(8);

        auto half = box.extent * 0.5f;
        auto right = glm::vec3{ half[0], 0, 0 };
        auto down = glm::vec3{ 0, half[1], 0 };
        auto under = glm::vec3{ 0, 0, half[2] };

        auto pos = box.position - half * .5f;

        std::array<aabb, 8> children_boxes = {
                aabb{ pos, half },
                aabb{ pos + right, half },
                aabb{ pos + down, half },
                aabb{ pos + right + down, half },
                aabb{ pos + under, half },
                aabb{ pos + under + right, half },
                aabb{ pos + under + down, half },
                aabb{ pos + under + right + down, half }
        };

        for (aabb& box : children_boxes)
        {
            children.emplace_back(box.position, box.extent);
        }
    }

    template <class... Ts>
    template<class ShapeT>
    octree<Ts...>* octree<Ts...>::where(const ShapeT& shape)
    {
        if (children.empty())
        {
            return this;
        }

        octree* ch = nullptr;

        for (auto& child : children)
        {
            using physics::intersect; // use ADL
            if (intersect(child.box, shape))
            {
                if (ch)
                {
                    return this;
                }
                ch = &child;
            }
        }

        if (!ch)
        {
            return nullptr;
        }

        return ch->where(shape);
    }

    template <class... Ts>
    template<class ShapeT>
    void octree<Ts...>::erase(const ShapeT* p)
    {
        constexpr auto index = png::index_of_t<ShapeT, types_list>();
        auto& v = boost::fusion::at_c<index>(ptr_vectors);
        v.erase(std::remove(v.begin(), v.end(), p));
        --size;
    }

    template <class... Ts>
    void octree<Ts...>::resize(const glm::vec3 &center, const glm::vec3 &extent) {
        Expects(children.size() == 0);
        box = {center, extent};
    }

    template <class octree_type, class HandlerT>
    void traverse_octree(const octree_type& otree, const physics::ray& ray, HandlerT fun)
    {
        std::queue<const octree_type*> q;

        using physics::intersect;
        auto put_octree = [&q, &ray](const octree_type* elem)
        {
            if (intersect(elem->bounding_box(), ray))
            {
                q.push(elem);
            }
        };

        put_octree(&otree);

        while (!q.empty())
        {
            const octree_type* oc = q.front();
            q.pop();

            for (auto& c : oc->get_children())
            {
                put_octree(&c);
            }

            oc->for_shapes(fun);
        };
    }
}
}

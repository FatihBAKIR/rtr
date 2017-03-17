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
#include <boost/circular_buffer.hpp>
#include <rtr_config.hpp>

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

        octree* parent;

        unsigned long size;
        unsigned long recursive_size;

        template <class ShapeT>
        void put(const ShapeT* p);

        template <class ShapeT>
        void erase(const ShapeT* p);

        template<class ShapeT>
        octree* insert_impl(const ShapeT& shape);

        template <class ShapeT>
        octree* where(const ShapeT& shape);

        void tighten();
    public:
        octree(const glm::vec3 &center, const glm::vec3 &extent, octree* parent = nullptr) : box(center, extent), size{}, parent(parent)
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

        void optimize();

        unsigned long get_size() const { return size; }
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

        auto tot = physics::merge(bounding_box(), p->bounding_box());
        resize(tot.position, tot.extent);

        if (get_size() > config::octree_cutoff && children.empty())
        {
            add_level();
        }
    }

    template <class... Ts>
    void octree<Ts...>::add_level() {
        Expects(children.empty());

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
            children.emplace_back(box.position, box.extent, this);
        }

        for_shapes([this](auto shape)
        {
            auto to = this->where(*shape);
            if (to != this)
            {
                Expects(to != nullptr);
                to->put(shape);
                this->erase(shape);
            }
        });
    }

    template <class... Ts>
    template<class ShapeT>
    octree<Ts...>* octree<Ts...>::where(const ShapeT& shape)
    {
        if (children.empty())
        {
            return this;
        }

        int count = 0;
        float dist = std::numeric_limits<float>::infinity();
        octree* ch = nullptr;

        for (auto& child : children)
        {
            using physics::intersect; // use ADL
            if (intersect(child.box, shape))
            {
                ++count;
                float new_dist = glm::length(child.box.position - shape.get_center());
                if (new_dist < dist)
                {
                    dist = new_dist;
                    ch = &child;
                }
            }
        }

        if (count == 0)
        {
            return this;
        }

        if (count > 3)
        {
            return this;
        }

        return ch->where(shape);
    }

    template <class... Ts>
    template<class ShapeT>
    void octree<Ts...>::erase(const ShapeT* p)
    {
        constexpr auto index = png::index_of_t<ShapeT, types_list>();
        auto& v = boost::fusion::at_c<index>(ptr_vectors);
        v.erase(std::remove(v.begin(), v.end(), p), v.end());
        --size;
    }

    template <class... Ts>
    void octree<Ts...>::resize(const glm::vec3 &center, const glm::vec3 &extent) {
        box = {center, extent};
        if (parent)
        {
            auto tot = merge(parent->bounding_box(), bounding_box());
            parent->resize(tot.position, tot.extent);
        }
    }

    template <class... Ts>
    void octree<Ts...>::optimize()
    {
        recursive_size = size;
        if (children.empty())
        {
            tighten();
            return;
        }
        std::for_each(children.begin(), children.end(), [&](auto& child) {
            child.optimize();
            recursive_size += child.recursive_size;
        });
        children.erase(std::remove_if(children.begin(), children.end(), [](auto& child){
            return child.recursive_size == 0;
        }), children.end());
        tighten();
    }

    template <class... Ts>
    void octree<Ts...>::tighten() {
        boost::optional<aabb> box;

        for (auto& child : get_children())
        {
            if (!box)
            {
                box = child.bounding_box();
                continue;
            }
            *box = merge(*box, child.bounding_box());
        }

        for_shapes([&](auto s_p)
        {
            if (!box)
            {
                box = s_p->bounding_box();
                return;
            }
            *box = merge(*box, s_p->bounding_box());
        });

        if (!box) return;
        resize(box->position, box->extent);
    }

    template <class octree_type, class HandlerT>
    void traverse_octree(const octree_type& otree, const physics::ray& ray, HandlerT fun)
    {
        thread_local boost::circular_buffer<const octree_type*> q(1024);

        auto put_octree = [&ray](const octree_type* elem)
        {
            using physics::intersect;
            if (intersect(elem->bounding_box(), ray))
            {
                q.push_back(elem);
            }
        };

        put_octree(&otree);

        while (!q.empty())
        {
            const octree_type* oc = q.front();
            q.pop_front();

            for (auto& c : oc->get_children())
            {
                put_octree(&c);
            }

            oc->for_shapes(fun);
        };
    }
}
}

//
// Created by fatih on 17.03.2017.
//

#pragma once

#include <physics/aabb.hpp>
#include <memory>

namespace rtr
{
    namespace physics
    {
        template <class T>
        struct bvh
        {
            aabb box;
            std::unique_ptr<bvh> left;
            std::unique_ptr<bvh> right;
            const T* obj;

            const aabb& bounding_box() const
            {
                return box;
            }
        };

        template <class bvh_type, class HandlerT>
        void traverse(const bvh_type& otree, const physics::ray& ray, HandlerT fun)
        {
            thread_local boost::circular_buffer<const bvh_type*> q(1024);

            auto put_octree = [&ray](const bvh_type* elem)
            {
                using physics::intersect;
                if (elem && intersect(elem->bounding_box(), ray))
                {
                    q.push_back(elem);
                }
            };

            put_octree(&otree);

            while (!q.empty())
            {
                const bvh_type* bvh = q.front();
                q.pop_front();

                if (bvh->obj)
                {
                    if (intersect(bvh->box, ray))
                    {
                        fun(bvh->obj);
                    }
                    continue;
                }

                put_octree(bvh->left.get());
                put_octree(bvh->right.get());
            };
        }
    }
}
//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <shapes/mesh.hpp>
#include <physics/octree.hpp>
#include <physics/collision.hpp>
#include <gsl/gsl>

namespace rtr
{
namespace physics
{
    struct octree_child
    {
        /*
         * using a regular enum so they decay into integers directly
         */
        enum
        {
            down_top_left,
            down_top_right,
            down_bot_left,
            down_bot_right,
            up_top_left,
            up_top_right,
            up_bot_left,
            up_bot_right
        };
    };

    static collide_result intersect(const aabb& box, const shapes::mesh& mesh)
    {
        return intersect(box, mesh.bounding_box());
    }

    void octree::insert(const shapes::sphere &sphere) {
        insert_impl(sphere);
    }

    void octree::insert(const shapes::mesh &mesh) {
        insert_impl(mesh);
    }

    template<class ShapeT>
    void octree::insert_impl(const ShapeT &shape) {
        if (children.empty())
        {
            put(&shape);
            return;
        }

        octree* ch = nullptr;

        for (auto& child : children)
        {
            if (physics::intersect(child.box, shape))
            {
                if (ch)
                {
                    /*
                     * at least 2 children intersect with this shape
                     * we need to put it here
                     */

                    put(&shape);
                    return;
                }
                ch = &child;
            }
        }

        if (!ch)
        {
            /*
             * the sphere didn't even intersect with us, how to handle?
             */
            return;
        }

        ch->insert_impl(shape);
    }

    void octree::put(const shapes::sphere *s) {
        spheres.push_back(s);
    }

    void octree::put(const shapes::mesh *m) {
        meshes.push_back(m);
    }

    void octree::add_level() {
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

    octree::octree(const glm::vec3 &center, const glm::vec3 &extent) : box(center, extent)
    {
    }

    octree::~octree() = default;
}
}

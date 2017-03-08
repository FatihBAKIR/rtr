//
// Created by fatih on 02.03.2017.
//

#include <iostream>
#include <queue>
#include <shapes/mesh.hpp>
#include <physics/ray.hpp>
#include <utility.hpp>

namespace rtr
{
namespace shapes
{
    physics::octree<triangle> partition(gsl::span<triangle> tris)
    {
        glm::vec3 min = tris[0].get_vertices()[0];
        glm::vec3 max = tris[0].get_vertices()[0];

        for (auto& tri : tris)
        {
            auto verts = tri.get_vertices();
            for (auto& vert : verts)
            {
                for (int j = 0; j < 3; ++j) {
                    if (min[j] > vert[j])
                    {
                        min[j] = vert[j];
                    }
                    if (max[j] < vert[j])
                    {
                        max[j] = vert[j];
                    }
                }
            }
        }

        std::cerr << "mesh min,max: " << min << ", " << max << '\n';

        const auto center = (max + min) * 0.5f;
        const auto extent = (max - min);

        std::cerr << "mesh octree: " << center << ", " << extent << '\n';

        physics::octree<triangle> partition(center, extent);

        for (auto& tri : tris)
        {
            auto oc = partition.insert(tri);

            if (oc->get_size() > 4 && oc->get_children().size() == 0)
            {
                oc->add_level();
            }
        }

        return partition;
    }


    physics::ray_hit mesh::intersect(const physics::ray& ray, float parameter) const
    {
        return physics::ray_hit{ ray, mat, {}, {}, parameter };
    }

    mesh::~mesh() = default;

    mesh::mesh(boost::container::vector<triangle> tris, const material* m)
            : tris(std::move(tris)), part(partition(this->tris)), mat(m)
    {
    }

    boost::optional<float> mesh::get_parameter(const rtr::physics::ray& ray) const
    {
        float cur_param = std::numeric_limits<float>::infinity();
        const triangle* cur_hit = nullptr;

        std::queue<const octree_type*> q;
        q.push(&part);

        using physics::intersect;

        while (!q.empty())
        {
            const octree_type* oc = q.front();
            q.pop();

            if (!intersect(oc->bounding_box(), ray))
            {
                continue;
            }

            for (auto& c : oc->get_children())
            {
                q.push(&c);
            }

            oc->for_shapes([&](auto shape)
            {
                auto p = shape->get_parameter(ray);
                if (p)
                {
                    auto param = *p;
                    if (param < cur_param)
                    {
                        cur_param = param;
                        cur_hit = shape;
                    }
                }
            });
        }

        if (!cur_hit)
        {
            return {};
        }

        return cur_param;
    }
}
}


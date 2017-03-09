//
// Created by fatih on 02.03.2017.
//

#include <iostream>
#include <queue>
#include <shapes/mesh.hpp>
#include <physics/ray.hpp>
#include <utility.hpp>
#include <chrono>

namespace rtr
{
namespace shapes
{
    physics::octree<triangle> partition(gsl::span<triangle> tris)
    {
        auto begin = std::chrono::high_resolution_clock::now();
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

            if (oc->get_size() > 1 && oc->get_children().size() == 0)
            {
                oc->add_level();
            }
        }

        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Octree generation took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms\n";

        return partition;
    }


    physics::ray_hit mesh::intersect(const physics::ray& ray, float parameter, const void* data) const
    {
        const triangle* tri = static_cast<const triangle*>(data);
        return physics::ray_hit{ ray, mat, ray.origin + ray.dir * parameter, tri->get_normal() , parameter };
    }

    mesh::~mesh() noexcept = default;

    mesh::mesh(boost::container::vector<triangle> tris, const material* m)
            : tris(std::move(tris)), part(partition(this->tris)), mat(m)
    {
    }

    boost::optional<mesh::param_result_t> mesh::get_parameter(const rtr::physics::ray& ray) const
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

        return { {cur_param, cur_hit} };
    }

    mesh::mesh(mesh && rhs) noexcept :
        tris(std::move(rhs.tris)),
        part(std::move(rhs.part)),
        mat(rhs.mat)
    {

    }
}
}


//
// Created by fatih on 02.03.2017.
//

#include <iostream>
#include <queue>
#include <geometry/mesh.hpp>
#include <physics/ray.hpp>
#include <utility.hpp>
#include <chrono>
#include <unordered_map>
#include <boost/container/static_vector.hpp>
#include <boost/container/flat_map.hpp>
#include <geometry.hpp>

#include <spdlog/spdlog.h>
#include <rtr_config.hpp>

#ifndef __linux__
#include <spdlog/fmt/ostr.h>
#endif

#include <physics/bvh.hpp>
#include <tbb/task_group.h>

namespace rtr
{
namespace geometry
{
    template <class IteratorT>
    std::unique_ptr<physics::bvh<triangle>> generate_bvh(IteratorT begin, IteratorT end, int axis = 0)
    {
        auto count = std::distance(begin, end);
        if (count == 0)
        {
            return nullptr;
        }
        if (count == 1)
        {
            return std::make_unique<physics::bvh<triangle>>(physics::bvh<triangle>{ (*begin)->bounding_box(), nullptr, nullptr, *begin });
        }

        auto center = std::next(begin, count / 2);
        std::nth_element(begin, center, end, [&](const triangle* tri_p, const triangle* tri2_p)
        {
            return tri_p->get_center()[axis] < tri2_p->get_center()[axis];
        });

        auto p2_count = std::distance(begin, center);

        tbb::task_group g;

        std::unique_ptr<physics::bvh<triangle>> p1_h, p2_h;
        g.run([&]{
            p1_h = generate_bvh(begin, center, (axis + 1) % 3);
        });
        g.run([&]{
            p2_h = generate_bvh(center, end, (axis + 1) % 3);
        });
        g.wait();

        auto bb = (p1_h && p2_h) ? physics::merge(p1_h->box, p2_h->box) : (!p1_h ? p2_h->box : p1_h->box);

        return std::make_unique<physics::bvh<triangle>>(physics::bvh<triangle>{ bb, std::move(p1_h), std::move(p2_h), nullptr });
    }

    auto make_bvh(gsl::span<triangle> tris)
    {
        static int cnt = 0;
        auto logger = spdlog::stderr_logger_st("mesh data " + std::to_string(++cnt));
        logger->info("Partitioning mesh into BVH");
        logger->info("Mesh has {0} tris", tris.size());

        auto begin = std::chrono::high_resolution_clock::now();

        std::vector<triangle*> ptrs;
        ptrs.reserve(tris.size());
        for (auto& tri : tris)
        {
            ptrs.push_back(&tri);
        }

        auto res = generate_bvh(ptrs.begin(), ptrs.end());
        auto end = std::chrono::high_resolution_clock::now();

        logger->info("Partitioning took {0} ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

        return res;
    }

    physics::octree<triangle> partition(gsl::span<triangle> tris)
    {
        static int cnt = 0;
        auto logger = spdlog::stderr_logger_st("mesh data " + std::to_string(++cnt));
        logger->info("Partitioning mesh into octree");
        logger->info("Mesh has {0} tris", tris.size());

        auto begin = std::chrono::high_resolution_clock::now();
        glm::vec3 min = tris[0].get_vertices()[0];
        glm::vec3 max = min;

        for (auto& tri : tris)
        {
            auto verts = tri.get_vertices();
            for (auto& vert : verts)
            {
                min = glm::min(min, vert);
                max = glm::max(max, vert);
            }
        }

        const auto center = (max + min) * 0.5f;
        const auto extent = glm::clamp(max - min, 0.01f, 1.f/0.f);

        logger->info("Octree: [{0}, {1}]", center, extent);

        physics::octree<triangle> partition(center, extent);

        for (auto& tri : tris)
        {
            auto oc = partition.insert(tri);
        }

        partition.optimize();

        auto end = std::chrono::high_resolution_clock::now();

        logger->info("Partitioning took {0} ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

        return partition;
    }

    physics::ray_hit mesh::intersect(const physics::ray& ray, float parameter, data_t& data) const
    {
        auto tri_index = data.tri - tris.data();
        glm::vec3 normal = data.tri->get_normal();
        glm::vec2 uv = {};

        if (vert_normals.size())
        {
            const auto& normal_1 = vert_normals[tri_index * 3];
            const auto& normal_2 = vert_normals[tri_index * 3 + 1];
            const auto& normal_3 = vert_normals[tri_index * 3 + 2];

            normal = glm::normalize(normal_1 * data.alpha + normal_2 * data.beta + normal_3 * data.gamma);
        }

        if (uvs.size())
        {
            const auto& uv_1 = uvs[face_indices[tri_index * 3]];
            const auto& uv_2 = uvs[face_indices[tri_index * 3 + 1]];
            const auto& uv_3 = uvs[face_indices[tri_index * 3 + 2]];

            uv= uv_1 * data.alpha +
                uv_2 * data.beta +
                uv_3 * data.gamma;
        }

        return physics::ray_hit{ this, ray, mat, ray.origin + ray.dir * parameter, normal, parameter, uv };
    }

    mesh::~mesh() noexcept = default;

    mesh::mesh(boost::container::vector<triangle> tris, bvector<int> indices, bvector<glm::vec2> uv, const material* m)
            : tris(std::move(tris)), face_indices(std::move(indices)), hier(make_bvh(this->tris)), mat(m), uvs(std::move(uv))
    {
    }

    boost::optional<mesh::param_res_t> mesh::get_parameter(const rtr::physics::ray& ray) const
    {
        triangle::param_res_t cur_param = {std::numeric_limits<float>::infinity(), {}};
        const triangle* cur_hit = nullptr;

        traverse(*hier, ray, [&](const triangle* tri)
        {
            auto p = tri->get_parameter(ray);
            if (p)
            {
                auto param = *p;
                if (param.parameter < cur_param.parameter)
                {
                    cur_param = param;
                    cur_hit = tri;
                }
            }
        });

        if (!cur_hit)
        {
            return {};
        }

        return { {cur_param.parameter, {cur_hit, cur_param.data.alpha, cur_param.data.beta, cur_param.data.gamma}} };
    }

    mesh::mesh(mesh && rhs) noexcept :
        tris(std::move(rhs.tris)),
        face_indices(std::move(rhs.face_indices)),
        vert_normals(std::move(rhs.vert_normals)),
        hier(std::move(rhs.hier)),
        uvs(std::move(rhs.uvs)),
        mat(rhs.mat)
    {
    }

    void mesh::smooth_normals()
    {
        using map_t = boost::container::flat_map<long, boost::container::vector<triangle*>>;
        map_t vertex_tris;

        for (std::size_t i = 0; i < face_indices.size(); ++i) {
            auto tri_index = i / 3;
            vertex_tris[face_indices[i]].push_back(&tris[tri_index]);
        }

        boost::container::flat_map<long, glm::vec3> normals;

        for (auto& elem : vertex_tris)
        {
            normals[elem.first] = {};
            for (auto& tri : elem.second)
            {
                normals[elem.first] += tri->get_normal() * tri->get_area();
            }
            normals[elem.first] = glm::normalize(normals[elem.first]);
        }

        vert_normals.clear();
        vert_normals.resize(tris.size() * 3);
        for (std::size_t i = 0; i < face_indices.size(); ++i)
        {
            vert_normals[i] = normals[face_indices[i]];
        }
    }
}
}


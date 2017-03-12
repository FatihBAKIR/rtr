//
// Created by fatih on 02.03.2017.
//

#include <iostream>
#include <queue>
#include <shapes/mesh.hpp>
#include <physics/ray.hpp>
#include <utility.hpp>
#include <chrono>
#include <unordered_map>
#include <boost/container/static_vector.hpp>
#include <boost/container/flat_map.hpp>
#include <shapes.hpp>

#include <spdlog/spdlog.h>

namespace rtr
{
namespace shapes
{
    physics::octree<triangle> partition(gsl::span<triangle> tris)
    {
        static int cnt = 0;
        auto logger = spdlog::stderr_logger_st("mesh data " + std::to_string(++cnt));
        logger->info("Partitioning mesh into octree");
        logger->info("Mesh has {0} tris", tris.size());

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

        const auto center = (max + min) * 0.5f;
        const auto extent = (max - min);

        logger->info("Octree: [{0}, {1}]", center, extent);

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

        logger->info("Partitioning took {0} ms", std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());

        return partition;
    }

    physics::ray_hit mesh::intersect(const physics::ray& ray, float parameter, data_t& data) const
    {
        glm::vec3 normal = data.tri->get_normal();

        if (vert_normals.size())
        {
            auto tri_index = data.tri - tris.data();
            const auto& normal_1 = vert_normals[tri_index * 3];
            const auto& normal_2 = vert_normals[tri_index * 3 + 1];
            const auto& normal_3 = vert_normals[tri_index * 3 + 2];

            normal = glm::normalize(normal_1 * data.alpha + normal_2 * data.beta + normal_3 * data.gamma);
        }

        return physics::ray_hit{ ray, mat, ray.origin + ray.dir * parameter, normal, parameter };
    }

    mesh::~mesh() noexcept = default;

    mesh::mesh(boost::container::vector<triangle> tris, bvector<long> indices, const material* m)
            : tris(std::move(tris)), face_indices(std::move(indices)), part(partition(this->tris)), mat(m)
    {
    }

    boost::optional<mesh::param_res_t> mesh::get_parameter(const rtr::physics::ray& ray) const
    {
        triangle::param_res_t cur_param = {std::numeric_limits<float>::infinity(), {}};
        const triangle* cur_hit = nullptr;

        traverse_octree(part, ray, [&](const triangle* tri)
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
        part(std::move(rhs.part)),
        face_indices(std::move(rhs.face_indices)),
        vert_normals(std::move(rhs.vert_normals)),
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
                normals[elem.first] += tri->get_normal();
            }
            normals[elem.first] /= (float)elem.second.size();
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


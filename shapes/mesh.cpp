//
// Created by fatih on 02.03.2017.
//

#include <iostream>
#include "mesh.hpp"

static std::ostream& operator<<(std::ostream& os, const glm::vec3& v)
{
    return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

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

            if (oc->get_size() > 4)
            {
                oc->add_level();
            }
        }

        return partition;
    }
}
}

rtr::shapes::mesh::~mesh()
{

}

rtr::shapes::mesh::mesh(boost::container::vector<rtr::shapes::triangle> tris)
    : tris(std::move(tris)), part(partition(this->tris))
{
}

boost::optional<float> rtr::shapes::mesh::get_parameter(const rtr::physics::ray& ray) const
{
    return boost::optional<float>();
}

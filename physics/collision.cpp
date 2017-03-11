//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <physics/collision.hpp>
#include <physics/aabb.hpp>
#include <shapes/sphere.hpp>
#include <shapes/mesh.hpp>
#include <physics/ray.hpp>
#include <cmath>

namespace rtr
{
namespace physics
{
    template <class T>
    constexpr auto sqr(const T& x)
    {
        return x * x;
    }

    class not_implemented_error : std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    collide_result intersect(const aabb& aabb, const shapes::sphere& sp)
    {
        float dmin = 0;

        const auto& center = sp.get_center();
        const auto& radius = sp.get_radius();
        auto r2 = radius * radius;

        const auto& bmin = aabb.position - (aabb.extent * 0.5f);
        const auto& bmax = aabb.position + (aabb.extent * 0.5f);

        for(auto i = 0; i < 3; ++i)
        {
            if(center[i] < bmin[i]) {
                dmin += sqr(center[i] - bmin[i]);
            }
            else if(center[i] > bmax[i]) {
                dmin += sqr(center[i] - bmax[i]);
            }
        }

        return dmin <= r2;

    }

    collide_result intersect(const aabb& a, const aabb& b)
    {
        if (std::abs(a.position[0] - b.position[0]) > (a.extent[0] + b.extent[0]) * 0.5f) return false;
        if (std::abs(a.position[1] - b.position[1]) > (a.extent[1] + b.extent[2]) * 0.5f) return false;
        if (std::abs(a.position[2] - b.position[2]) > (a.extent[2] + b.extent[2]) * 0.5f) return false;
        return true;
    }

    collide_result intersect(const aabb& box, const shapes::mesh& mesh)
    {
        return intersect(box, mesh.bounding_box());
    }

    collide_result intersect(const aabb& box, const ray& ray)
    {
        const auto inv = ray.get_inverse();

        const auto& diff1 = box.min - ray.origin;
        const auto& diff2 = box.max - ray.origin;
        const auto& t = diff1 * inv;
        const auto& tt = diff2 * inv;

        auto& t1 = t[0];
        auto& t2 = tt[0];

        float tMin = std::min(t1, t2);
        float tMax = std::max(t1, t2);

        for (int i = 1; i < 3; ++i) {
            auto& t1 = t[i];
            auto& t2 = tt[i];

            tMin = std::max(tMin, std::min(t1, t2));
            tMax = std::min(tMax, std::max(t1, t2));
        }

        return tMax >= std::max(.0f, tMin);
    }
}
}
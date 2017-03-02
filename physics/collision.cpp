//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <physics/collision.hpp>
#include <physics/aabb.hpp>
#include <shapes/sphere.hpp>
#include <shapes/triangle.hpp>
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

    collide_result intersect(const rtr::aabb& aabb, const rtr::sphere& sp)
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

    collide_result intersect(const rtr::aabb& a, const rtr::aabb& b)
    {
        if (std::abs(a.position[0] - b.position[0]) > (a.extent[0] + b.extent[0]) * 0.5f) return false;
        if (std::abs(a.position[1] - b.position[1]) > (a.extent[1] + b.extent[2]) * 0.5f) return false;
        if (std::abs(a.position[2] - b.position[2]) > (a.extent[2] + b.extent[2]) * 0.5f) return false;
        return true;
    }

    /*collide_result intersect(const rtr::aabb&, const rtr::triangle& tri)
    {
        throw not_implemented_error("aabb triangle intersect not implemented");
    }*/

    collide_result intersect(const rtr::aabb& box, const rtr::ray& ray)
    {
        using std::min;
        using std::max;

        const auto inv = glm::vec3(1, 1, 1) / ray.dir;

        float t1 = (box.min[0] - ray.origin[0]) * inv[0];
        float t2 = (box.max[0] - ray.origin[0]) * inv[0];

        float tMin = min(t1, t2);
        float tMax = max(t1, t2);

        for (int i = 1; i < 3; ++i) {
            t1 = (box.min[i] - ray.origin[i]) * inv[i];
            t2 = (box.max[i] - ray.origin[i]) * inv[i];

            tMin = max(tMin, min(t1, t2));
            tMax = min(tMax, max(t1, t2));
        }

        return tMax >= max(tMin, 0.0f);
    }
}
}
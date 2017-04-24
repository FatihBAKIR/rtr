//
// Created by fatih on 4/23/17.
//

#include "perlin2d.hpp"
#include <random>
#include <vector>
#include <algorithm>

namespace rtr
{
namespace texturing
{
    template <class T>
    T weight(const T& t)
    {
        auto abs_t = std::abs(t);
        auto abs_t_3 = abs_t * abs_t * abs_t;

        return -6 * abs_t_3 * abs_t * abs_t +
                15 * abs_t_3 * abs_t -
                10 * abs_t_3 + 1;
    }

    class perlin2d_impl
    {
        std::default_random_engine rng;
        std::vector<glm::vec3> big_g = {
                {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
                {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
                {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1},
                {1, 1, 0}, {-1, 1, 0}, {0, -1, 1}, {0, -1, -1}
        };
        std::vector<int> P;
        int N = 16;

        int phi(int x)
        {
            return P[std::abs(x) % N];
        }

        glm::vec3 small_g(int i, int j, int k)
        {
            return big_g[phi(i + phi(j + phi(k)))];
        }

        float omega(int i, int j, int k, const glm::vec3& v)
        {
            return weight(v.x) * weight(v.y) * weight(v.z) * glm::dot(small_g(i, j, k), v);
        }

    public:

        float scaling;
        perlin_appearance app;

        perlin2d_impl()
        {
            P.resize(N);
            std::iota(P.begin(), P.end(), 0);
            std::shuffle(P.begin(), P.end(), rng);
        }

        float noise(const glm::vec3& v)
        {
            float n = 0;

            int floor_x = int(std::floor(v.x));
            int floor_y = int(std::floor(v.y));
            int floor_z = int(std::floor(v.z));

            for (int i = floor_x; i <= floor_x + 1; ++i)
            {
                for (int j = floor_y; j <= floor_y + 1; ++j)
                {
                    for (int k = floor_z; k <= floor_z + 1; ++k)
                    {
                        n += omega(i, j, k, v - glm::vec3(i, j, k));
                    }
                }
            }

            return n;
        }
    };

    perlin2d::perlin2d(perlin_appearance a, float scale) :
        pimpl(std::make_unique<perlin2d_impl>()) {
        pimpl->app = a;
        pimpl->scaling = scale;
    }

    glm::vec3 perlin2d::sample(const glm::vec3& pos) const
    {
        auto r = pimpl->noise(pos * pimpl->scaling);
        return glm::vec3(pimpl->app == perlin_appearance::vein ? std::abs(r) : ((r + 1) /2));
    }

    void perlin2d::set_sampling_mode(rtr::texturing::sampling_mode mode)
    {
        sampler2d::set_sampling_mode(mode);
    }

    perlin2d::~perlin2d() = default;
}
}


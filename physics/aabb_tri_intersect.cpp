/*
 * Ripped off from: http://fileadmin.cs.lth.se/cs/Personal/Tomas_Akenine-Moller/code/tribox3.txt
 * Refactored to use glm vectors and gsl spans, but it still uses a lot of macros for some reason
 */

#include <cmath>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include <physics/aabb.hpp>
#include <array>
#include <tuple>
#include <gsl/span>

namespace {
    enum {
        X, Y, Z
    };

    template<class T>
    auto find_min_max(const T &a, const T &b, const T &c) {
        auto min = std::min(a, b);
        min = std::min(min, c);

        auto max = std::max(a, b);
        max = std::max(max, c);

        return std::make_pair(min, max);
    }

    int planeBoxOverlap(const glm::vec3 &normal, const glm::vec3 &vert, const glm::vec3 &maxbox)    // -NJMP-
    {
        glm::vec3 vmin, vmax;
        for (int q = 0; q <= 2; q++) {
            float v = vert[q];                    // -NJMP-
            if (normal[q] > 0.0f) {
                vmin[q] = -maxbox[q] - v;    // -NJMP-
                vmax[q] = maxbox[q] - v;    // -NJMP-
            } else {
                vmin[q] = maxbox[q] - v;    // -NJMP-
                vmax[q] = -maxbox[q] - v;    // -NJMP-
            }
        }

        if (glm::dot(normal, vmin) > 0.0f) return false;    // -NJMP-
        if (glm::dot(normal, vmax) >= 0.0f) return true;    // -NJMP-

        return false;
    }

#define AXISTEST_X01(a, b, fa, fb)               \
    p0 = a*v0[Y] - b*v0[Z];                       \
    p2 = a*v2[Y] - b*v2[Z];                       \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)               \
    p0 = a*v0[Y] - b*v0[Z];                       \
    p1 = a*v1[Y] - b*v1[Z];                       \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Y02(a, b, fa, fb)               \
    p0 = -a*v0[X] + b*v0[Z];                   \
    p2 = -a*v2[X] + b*v2[Z];                       \
        if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)               \
    p0 = -a*v0[X] + b*v0[Z];                   \
    p1 = -a*v1[X] + b*v1[Z];                       \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Z12(a, b, fa, fb)               \
    p1 = a*v1[X] - b*v1[Y];                       \
    p2 = a*v2[X] - b*v2[Y];                       \
        if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)               \
    p0 = a*v0[X] - b*v0[Y];                   \
    p1 = a*v1[X] - b*v1[Y];                       \
        if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
    if(min>rad || max<-rad) return 0;
}

namespace rtr {
    namespace physics {
        bool intersect(const rtr::physics::aabb &box, gsl::span<const glm::vec3> triverts) {
            float min, max, p0, p1, p2, rad, fex, fey, fez;

            auto boxhalfsize = box.extent * 0.5f;

            auto v0 = triverts[0] - box.position;
            auto v1 = triverts[1] - box.position;
            auto v2 = triverts[2] - box.position;

            auto e0 = v1 - v0;
            auto e1 = v2 - v1;
            auto e2 = v0 - v2;

            fex = std::abs(e0[X]);
            fey = std::abs(e0[Y]);
            fez = std::abs(e0[Z]);

            AXISTEST_X01(e0[Z], e0[Y], fez, fey);
            AXISTEST_Y02(e0[Z], e0[X], fez, fex);
            AXISTEST_Z12(e0[Y], e0[X], fey, fex);

            fex = std::abs(e1[X]);
            fey = std::abs(e1[Y]);
            fez = std::abs(e1[Z]);

            AXISTEST_X01(e1[Z], e1[Y], fez, fey);
            AXISTEST_Y02(e1[Z], e1[X], fez, fex);
            AXISTEST_Z0(e1[Y], e1[X], fey, fex);

            fex = std::abs(e2[X]);
            fey = std::abs(e2[Y]);
            fez = std::abs(e2[Z]);

            AXISTEST_X2(e2[Z], e2[Y], fez, fey);
            AXISTEST_Y1(e2[Z], e2[X], fez, fex);
            AXISTEST_Z12(e2[Y], e2[X], fey, fex);

            std::tie(min, max) = find_min_max(v0[X], v1[X], v2[X]);

            if (min > boxhalfsize[X] || max < -boxhalfsize[X]) return false;

            std::tie(min, max) = find_min_max(v0[Y], v1[Y], v2[Y]);

            if (min > boxhalfsize[Y] || max < -boxhalfsize[Y]) return false;

            std::tie(min, max) = find_min_max(v0[Z], v1[Z], v2[Z]);

            if (min > boxhalfsize[Z] || max < -boxhalfsize[Z]) return false;

            auto normal = glm::cross(e0, e1);

            if (!planeBoxOverlap(normal, v0, boxhalfsize)) return false;    // -NJMP-

            return true;
        }
    }
}

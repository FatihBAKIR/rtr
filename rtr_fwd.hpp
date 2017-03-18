//
// Created by Mehmet Fatih BAKIR on 26/02/2017.
//

#include <boost/container/container_fwd.hpp>

namespace rtr {
    template<class T>
    using bvector = boost::container::vector<T>;

    struct shading_ctx;

    class material;
    class rt_mat;

    struct vertex;

    class scene;
    class camera;

    namespace geometry {
        class sphere;

        class triangle;

        class mesh;
    }

    namespace physics {
        struct ray;
        struct ray_hit;

        struct aabb;

        template<class...>
        class octree;

        template <class T>
        class bvh;
    }

    namespace lights
    {
        class ambient_light;
        class point_light;
    }
}

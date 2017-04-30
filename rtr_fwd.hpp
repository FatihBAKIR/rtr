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
    class normal_mat;
    namespace shading
    {
        class toon_shader;
        class mirror_material;
        class glass;
        class bump;
    }

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

    namespace texturing
    {
        class sampler2d;
        template <class, int> class tex2d;
        class perlin2d;
    }

    namespace lights
    {
        class ambient_light;
        class point_light;
        class spot_light;
        class area_light;
        class directional_light;
    }
}

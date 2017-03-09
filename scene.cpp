//
// Created by fatih on 02.03.2017.
//

#include <scene.hpp>
#include <physics/ray.hpp>
#include <shapes/mesh.hpp>
#include <shapes/sphere.hpp>
#include <vertex.hpp>
#include <queue>

boost::optional<rtr::physics::ray_hit> rtr::scene::ray_cast(const rtr::physics::ray &ray) const {
    using const_shapes = png::map_t<png::mapper<std::add_const_t>, shape_list>;
    using shape_pointers = png::map_t<png::mapper<std::add_pointer_t>, const_shapes>;
    using shape_variant = png::convert_t<boost::variant, shape_pointers>;

    boost::optional<shape_variant> cur_hit;
    float cur_param = std::numeric_limits<float>::infinity();
    const void* shape_data = nullptr;

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
                auto& param = *p;
                if (param.parameter < cur_param)
                {
                    cur_param = param.parameter;
                    shape_data = param.data;
                    cur_hit = shape;
                }
            }
        });
    }

    if (!cur_hit)
    {
        return {};
    }

    boost::optional<rtr::physics::ray_hit> res;
    boost::apply_visitor([&](auto shape)
    {
        res = shape->intersect(ray, cur_param, shape_data);
    }, *cur_hit);
    return res;
}

rtr::scene::scene(const glm::vec3& c, const glm::vec3& e, const std::unordered_map<long, rtr::material> mats)
        : part(c, e), mats(std::move(mats))
{
}

void rtr::scene::finalize()
{

    boost::fusion::for_each(shapes, [&](auto& vector)
    {
        std::for_each(vector.begin(), vector.end(), [&](auto& elem)
        {
            part.insert(elem);
        });
    });
}

rtr::scene::~scene() = default;
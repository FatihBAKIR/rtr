//
// Created by fatih on 02.03.2017.
//

#include <lights.hpp>
#include <shapes.hpp>

#include <scene.hpp>
#include <physics/ray.hpp>
#include <vertex.hpp>
#include <queue>

bool rtr::scene::ray_cast_param(const physics::ray& ray, float min_param, float max_param) const
{
    bool res = false;
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
                if (param.parameter < max_param && param.parameter > min_param)
                {
                    res = true;
                    decltype(q) empty;
                    q.swap(empty);
                }
            }
        });
    }

    return res;
}

template <class ShapeT>
using shape_ptr_data_tuple = std::tuple<const ShapeT*, decltype(std::declval<typename ShapeT::param_res_t>().data)>;

boost::optional<rtr::physics::ray_hit> rtr::scene::ray_cast(const rtr::physics::ray &ray) const {

    using shape_pointers = png::map_t<png::mapper<shape_ptr_data_tuple>, shape_list>;
    using shape_variant = png::convert_t<boost::variant, shape_pointers>;

    boost::optional<shape_variant> cur_hit;
    float cur_param = std::numeric_limits<float>::infinity();

    traverse_octree(part, ray, [&](auto* shape)
    {
        auto p = shape->get_parameter(ray);
        if (p)
        {
            auto& param = *p;
            if (param.parameter < cur_param)
            {
                cur_param = param.parameter;
                cur_hit = std::make_tuple(shape, param.data);
            }
        }
    });

    if (!cur_hit)
    {
        return {};
    }

    boost::optional<rtr::physics::ray_hit> res;
    boost::apply_visitor([&](auto hit)
    {
        auto& shape = std::get<0>(hit);
        auto& data = std::get<1>(hit);
        res = shape->intersect(ray, cur_param, data);
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

void rtr::scene::insert(const rtr::lights::ambient_light& light)
{
    ambient = std::make_unique<lights::ambient_light>(light);
}

void rtr::scene::insert(rtr::lights::ambient_light&& light)
{
    ambient = std::make_unique<lights::ambient_light>(std::move(light));
}

void rtr::scene::resize(const glm::vec3 &pos, const glm::vec3 &extent) {
    part.resize(pos, extent);
}

rtr::scene::~scene() = default;

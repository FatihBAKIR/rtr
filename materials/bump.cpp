//
// Created by fatih on 4/29/17.
//

#include <physics/ray.hpp>
#include <texturing/tex2d.hpp>
#include <utility.hpp>
#include "bump.hpp"

namespace rtr
{
namespace shading
{
    float luminance(glm::vec3 rgb)
    {
        return rgb.x + rgb.y + rgb.z;
    }

    glm::vec3 bump::shade(const shading_ctx& ctx) const
    {
        auto tex2d = static_cast<const texturing::tex2d<uint8_t, 3>*>(bump_map);
        auto expander = glm::vec2(tex2d->get_width(), tex2d->get_height());

        auto uv = ctx.hit.uv * expander;

        auto dddu_rgb = tex2d->sample(std::round(uv.x) + 1, std::round(uv.y)) - tex2d->sample(std::round(uv.x), std::round(uv.y));
        auto dddv_rgb = tex2d->sample(std::round(uv.x), std::round(uv.y) + 1) - tex2d->sample(std::round(uv.x), std::round(uv.y));

        float dddu = luminance(dddu_rgb) * 75;
        float dddv = luminance(dddv_rgb) * 75;

        auto dqdu = ctx.hit.dpdu + dddu * ctx.hit.normal;
        auto dqdv = ctx.hit.dpdv + dddv * ctx.hit.normal;

        glm::vec3 new_normal;
        boost::apply_visitor(make_lambda_visitor<void>(
                [&](const geometry::mesh* m)
                {
                    new_normal = glm::normalize(glm::cross(dqdv, dqdu));
                },
                [&](const geometry::sphere* s)
                {
                    new_normal = glm::normalize(glm::cross(dqdu, dqdv));
                }
        ), ctx.hit.shape);

        auto new_hit = ctx.hit;
        new_hit.normal = new_normal;
        return base->shade({ctx.scn, ctx.view_dir, new_hit});
    }
}
}


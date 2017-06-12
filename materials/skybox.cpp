//
// Created by fatih on 6/11/17.
//

#include <physics/ray.hpp>
#include <materials/skybox.hpp>
#include <texturing/sampler.hpp>
#include <geometry/sphere.hpp>
#include <glm/gtx/norm.hpp>
#include <boost/math/constants/constants.hpp>

namespace rtr
{
namespace shading
{
    static constexpr auto pi = boost::math::constants::pi<float>();
    glm::vec3 skybox::shade(const shading_ctx& ctx) const
    {
        auto sphere = boost::get<const geometry::sphere*>(ctx.hit.shape);
        auto diff = glm::normalize(ctx.hit.position - sphere->get_center());

        auto r = (std::acos(diff.z) / pi) / std::sqrt(diff.x * diff.x + diff.y * diff.y);

        auto actual_uv = ((glm::vec2{diff.x, diff.y} * r) + glm::vec2{1, 1}) * 0.5f;
        actual_uv.y *= -1;
        auto color = m_light_probe->sample(actual_uv) * (m_scale);
        return color;
    }
}
}


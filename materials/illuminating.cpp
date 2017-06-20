//
// Created by fatih on 6/1/17.
//

#include <physics/ray.hpp>
#include <glm/gtx/norm.hpp>
#include <materials/illuminating.hpp>

namespace rtr
{
namespace shading
{
    glm::vec3 illuminating::shade(const shading_ctx& ctx) const
    {
        return m_intensity;
    }

    illuminating::illuminating(const glm::vec3& rad)
        : m_intensity(rad)
    {

    }
}
}


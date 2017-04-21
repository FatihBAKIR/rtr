//
// Created by fatih on 4/19/17.
//

#ifndef RTR_METAL_HPP
#define RTR_METAL_HPP

#include <materials/shading_ctx.hpp>
#include <materials/rt_mat.hpp>

namespace rtr {
namespace shading {
    class metal : public material
    {
        material* base;

        float rough;
        glm::vec3 reflectance;

    public:
        metal(const rt_mat& m, const glm::vec3& refl, float roughness)
                : base(new rt_mat(m)), reflectance(refl), rough(roughness)
        { }

        glm::vec3 shade(const shading_ctx&) const override;
    };
}
}

#endif //RTR_METAL_HPP

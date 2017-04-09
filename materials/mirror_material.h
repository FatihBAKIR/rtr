//
// Created by fatih on 09.04.2017.
//

#pragma once

#include <materials/material.hpp>
#include <materials/rt_mat.hpp>

namespace rtr {
    namespace shading {
        class mirror_material : public material {
            rt_mat base;

            glm::vec3 reflectance;

        public:
            mirror_material(const rt_mat& m, const glm::vec3& refl) :
                base(m), reflectance(refl) {}

            glm::vec3 shade(const shading_ctx&) const override;
        };
    }

}



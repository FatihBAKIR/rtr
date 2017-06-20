//
// Created by fatih on 5/13/17.
//

#pragma once

#include <glm/vec3.hpp>
#include "brdf_common.hpp"

namespace rtr
{
    namespace brdf
    {
        class phong
        {
            float phong_coeff;
        public:
            phong(float coeff) : phong_coeff(coeff){}
            glm::vec3 calculate(const brdf_data*, const brdf_ctx*) const;
        };

        class phong_modified
        {
            float phong_coeff;
        public:
            phong_modified(float coeff) : phong_coeff(coeff){}
            glm::vec3 calculate(const brdf_data*, const brdf_ctx*) const;
        };

        class phong_modified_normalized
        {
            float phong_coeff;
        public:
            phong_modified_normalized(float coeff) : phong_coeff(coeff){}
            glm::vec3 calculate(const brdf_data*, const brdf_ctx*) const;
        };
    }
}



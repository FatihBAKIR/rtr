//
// Created by Mehmet Fatih BAKIR on 14/05/2017.
//

#pragma once

#include <glm/vec3.hpp>
#include <brdf/brdf_common.hpp>

namespace rtr
{
    namespace brdf
    {
        class blinn_phong
        {
            float phong_coeff;
        public:
            blinn_phong(float coeff) : phong_coeff(coeff){}
            glm::vec3 calculate(const brdf_data*, const brdf_ctx*) const;
        };

        class blinn_phong_modified
        {
            float phong_coeff;
        public:
            blinn_phong_modified(float coeff) : phong_coeff(coeff){}
            glm::vec3 calculate(const brdf_data*, const brdf_ctx*) const;
        };

        class blinn_phong_modified_normalized
        {
            float phong_coeff;
        public:
            blinn_phong_modified_normalized(float coeff) : phong_coeff(coeff){}
            glm::vec3 calculate(const brdf_data*, const brdf_ctx*) const;
        };
    }
}



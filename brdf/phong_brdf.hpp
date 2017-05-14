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
        public:
            glm::vec3 calculate(const brdf_data*, const brdf_ctx*) const;
        };
    }
}



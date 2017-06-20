//
// Created by Mehmet Fatih BAKIR on 14/05/2017.
//

#include <glm/vec3.hpp>
#include "brdf_common.hpp"

namespace rtr
{
namespace brdf
{
    class torrance_sparrow
    {
        float exponent;
    public:
        torrance_sparrow(float a) : exponent(a){}
        glm::vec3 calculate(const brdf_data*, const brdf_ctx*) const;
    };
}
}


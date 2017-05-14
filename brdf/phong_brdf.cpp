//
// Created by fatih on 5/13/17.
//

#include <boost/math/constants/constants.hpp>
#include "phong_brdf.hpp"

glm::vec3 rtr::brdf::phong::calculate(const rtr::brdf::brdf_data* data, const rtr::brdf::brdf_ctx* ctx) const
{
    return data->diffuse / boost::math::constants::pi<float>();
}

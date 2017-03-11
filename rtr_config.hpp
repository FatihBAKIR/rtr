//
// Created by fatih on 02.03.2017.
//

#pragma once

namespace rtr
{
namespace config
{
    #if defined(RTR_OPENEXR_SUPPORT) && RTR_OPENEXR_SUPPORT
        constexpr bool OpenEXR_Support = true;
    #else
        constexpr bool OpenEXR_Support = false;
    #endif

    using position_t = glm::vec3;
    using vector_t = glm::vec3;
}
}


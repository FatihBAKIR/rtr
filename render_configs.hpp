//
// Created by Mehmet Fatih BAKIR on 10/03/2017.
//

#pragma once

#if RTR_OPENEXR_SUPPORT
    #include <gil_extension/exr/half/typedefs.hpp>
#endif

namespace rtr
{
namespace render_config
{

template <class>
struct render_traits;

#if RTR_OPENEXR_SUPPORT
    struct hdr_render;
    template <>
    struct render_traits<hdr_render>
    {
        using image_type = boost::gil::rgb16f_image_t;
    };
#endif

    struct ldr_render;
    template <>
    struct render_traits<ldr_render>
    {
        using image_type = boost::gil::rgb8_image_t;
    };
}
}

//
// Created by Mehmet Fatih BAKIR on 10/03/2017.
//

#pragma once

#if RTR_OPENEXR_SUPPORT
    #include <gil_extension/exr/half/typedefs.hpp>
#include <gil_extension/exr/exr_io.hpp>
#endif

#if RTR_PNG_SUPPORT
    #include <boost/gil/extension/io/png_io.hpp>
#endif

#include <boost/gil/gil_all.hpp>

#include <glm/glm.hpp>

namespace rtr
{
namespace render_config
{

template <class>
struct render_traits;

#if RTR_OPENEXR_SUPPORT
    struct hdr_render
    {
        static constexpr auto name = "HDR Render";
        static glm::vec3 process(const glm::vec3& v)
        {
            return v;
        }
    };
    template <>
    struct render_traits<hdr_render>
    {
        using image_type = boost::gil::rgb16f_image_t;
    };
#endif

    struct ldr_render
    {
        static constexpr auto name = "LDR Render";
        static glm::vec3 process(const glm::vec3& v)
        {
            return glm::clamp(v, 0.f, 255.f);
        }
    };

    template <>
    struct render_traits<ldr_render>
    {
        using image_type = boost::gil::rgb8_image_t;
    };
}
}

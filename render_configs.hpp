//
// Created by Mehmet Fatih BAKIR on 10/03/2017.
//

#pragma once

#include <gil_extension/exr/half/typedefs.hpp>

namespace rtr
{
namespace render_config
{
struct hdr_render;
struct ldr_render;

template <class>
struct render_traits;

template <>
struct render_traits<hdr_render>
{
    using image_type = boost::gil::rgb16f_image_t;
};

template <>
struct render_traits<ldr_render>
{
    using image_type = boost::gil::rgb8_image_t;
};
}
}

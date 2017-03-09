//
// Created by fatih on 09.03.2017.
//

#include <gil_extension/exr/detail/color_space_traits.hpp>

namespace boost
{
namespace gil
{

namespace detail {
    const char *exr_color_space_traits<gil::gray_t>::channel_name[1] = {"Y"};
    const char *exr_color_space_traits<gil::rgb_t>::channel_name[3] = {"R", "G", "B"};
    const char *exr_color_space_traits<gil::rgba_t>::channel_name[4] = {"R", "G", "B", "A"};

}}}
//
// Created by fatih on 12.03.2017.
//

#include <render_configs.hpp>

#if RTR_OPENEXR_SUPPORT
constexpr const char* rtr::render_config::hdr_render::name;
#endif

#if RTR_PNG_SUPPORT
constexpr const char* rtr::render_config::ldr_render::name;
#endif
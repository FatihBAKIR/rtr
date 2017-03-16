//
// Created by fatih on 02.03.2017.
//

#pragma once

namespace rtr {
    namespace config {
        static constexpr const char *build_types[] = {
                "Debug",
                "Performance"
        };

#if defined(RTR_OPENEXR_SUPPORT) && RTR_OPENEXR_SUPPORT
        constexpr bool openexr_support = true;
#else
        constexpr bool openexr_support = false;
#endif

#if !defined(RTR_NO_THREADING) || RTR_NO_THREADING == 0
        constexpr bool thread_support = true;
#undef RTR_NO_THREADING
#define RTR_NO_THREADING 0
#else
        constexpr bool thread_support = false;
#endif

        constexpr auto build_type = build_types[RTR_BUILD_TYPE];

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define RTR_VERSION_STRING STR(RTR_VERSION_MAJOR) "." STR(RTR_VERSION_MINOR) "." STR(RTR_VERSION_PATCH)
        constexpr auto version_str = RTR_VERSION_STRING;
    }
}

#undef STR
#undef STR_HELPER
# - Try to find catch framework
if(spdlog_FOUND)
    return()
endif()

find_path(SPDLOG_INCLUDE_DIR spdlog/spdlog.h HINT ${EXTERNAL_ROOT}/include)

set(SPDLOG_INCLUDE_DIRS ${SPDLOG_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(spdlog  DEFAULT_MSG SPDLOG_INCLUDE_DIR)

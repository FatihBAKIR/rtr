cmake_minimum_required(VERSION 3.5)
project(rtr)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

set(CMAKE_CXX_STANDARD 14)

set(DEBUG_FLAGS "-g -O0")
set(RELEASE_FLAGS "-Ofast -march=native -fno-stack-protector")

add_definitions(-DRTR_VERSION_MAJOR=0)
add_definitions(-DRTR_VERSION_MINOR=2)
add_definitions(-DRTR_VERSION_PATCH=0)
add_definitions(-Dint_p_NULL=nullptr)
add_definitions(-DTBB_USE_EXCEPTIONS=0)

add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

if (CMAKE_BUILD_TYPE MATCHES "Release")
    message(STATUS "Performance Build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS}")
    include(FindSSE)
    if (HAVE_SSE4_2_EXTENSIONS)
        message(STATUS "Target has SSE4.2")
        add_definitions(-DGLM_FORCE_SSE42)
        add_definitions(-DGLM_FORCE_ALIGNED)
    endif()
    add_definitions(-DRTR_BUILD_TYPE=1)
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    message("Debug Build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS}")
    add_definitions(-DRTR_BUILD_TYPE=0)
endif()

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
endif()

set (LIGHT_FILES lights/point_light.hpp lights/ambient_light.hpp lights/spot_light.hpp materials/illuminating.cpp materials/illuminating.hpp)
set (GEOMETRY_FILES
        geometry/mesh.hpp geometry/mesh.cpp
        geometry/sphere.hpp geometry/sphere.cpp
        geometry/triangle.hpp geometry/triangle.cpp)
set (PHYSICS_FILES
        physics/ray.cpp physics/ray.hpp
        physics/octree.hpp
        physics/collision.hpp physics/collision.cpp
        physics/aabb.hpp
        physics/aabb_tri_intersect.cpp physics/bvh.hpp)

set (TRANSFORM_FILES transform/transform.cpp transform/transform.hpp)

set (COMMON_FILES ${LIGHT_FILES} ${GEOMETRY_FILES} ${PHYSICS_FILES} ${TRANSFORM_FILES}
        camera.cpp camera.hpp
        transform.hpp
        scene.hpp scene.cpp
        rtr_fwd.hpp
        rtr_config.hpp
        vertex.hpp
        meta_list.hpp
        xml_parse.cpp xml_parse.hpp
        utility.hpp utility.cpp
        lights.hpp
        geometry.hpp render_configs.hpp static_storage.cpp
        materials/shading_ctx.hpp
        materials/material.hpp
        materials/rt_mat.hpp materials/rt_mat.cpp
        materials/normal_mat.cpp materials/normal_mat.hpp)

set (GIL_HDR_FILES gil_extension/exr/detail/color_space_traits.cpp)

set (TEST_FILES ${GIL_HDR_FILES} ${COMMON_FILES} tests/physics/collisions.cpp tests/tests_main.cpp tests/physics/bvh_tests.cpp)

set (SOURCE_FILES ${GIL_HDR_FILES} ${COMMON_FILES} main.cpp materials/toon_shading.cpp materials/toon_shading.hpp
        materials/mirror_material.cpp materials/mirror_material.h materials/glass.cpp materials/glass.h lights/spot_light.hpp lights/area_light.hpp materials/metal.cpp materials/metal.hpp texturing/sampler.cpp texturing/sampler.hpp texturing/tex2d.cpp texturing/tex2d.hpp texturing/perlin2d.cpp texturing/perlin2d.hpp lights/directional_light.hpp materials/bump.cpp materials/bump.hpp brdf/phong_brdf.cpp brdf/phong_brdf.hpp materials/brdf_mat.cpp materials/brdf_mat.hpp brdf/brdf_common.hpp brdf/blinn_phong_brdf.cpp brdf/blinn_phong_brdf.hpp brdf/torrance_sparrow.cpp brdf/torrance_sparrow.hpp materials/skybox.cpp materials/skybox.hpp merge_ibl.cpp)

if(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wno-long-long -pedantic")
elseif(CMAKE_C_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wno-long-long -pedantic -Wno-missing-braces")
endif()

include(cotire)
add_executable(ray ${SOURCE_FILES})
add_executable(tests ${TEST_FILES})

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS system thread filesystem program_options)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(OpenEXR)
find_package(Spdlog REQUIRED)
find_package(Curses)
find_package(assimp REQUIRED)
find_package(TinyXML2 REQUIRED)
find_package(GLM REQUIRED)
find_package(OpenCV REQUIRED)

set(TBB_ROOT_DIR "/home/fatih/tbb")
set(TBB_USE_DEBUG_BUILD False)

find_package(TBB)

target_include_directories(ray PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(ray ${Boost_LIBRARIES})
target_include_directories(ray PUBLIC ${Boost_INCLUDE_DIRS})

message(STATUS ${TINYXML2_LIBRARIES})

target_link_libraries(ray ${TINYXML2_LIBRARIES})
target_include_directories(ray PUBLIC ${TINYXML2_INCLUDE_DIRS})

target_include_directories(ray PUBLIC 3rd_party/gsl/include)
target_include_directories(ray PUBLIC ${GLM_INCLUDE_DIRS})

if (OpenCV_FOUND)
    message(STATUS "Found OpenCV")
    target_include_directories(ray PUBLIC ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(ray ${OpenCV_LIBS})

    target_compile_definitions(ray PUBLIC RTR_OPENCV_SUPPORT=1)
endif()

if (assimp_FOUND)
    message(STATUS "Found assimp")

    target_compile_definitions(ray PRIVATE RTR_ASSIMP_SUPPORT=1)

    target_include_directories(ray PUBLIC ${assimp_INCLUDE_DIRS})
    target_link_libraries(ray ${assimp_LIBRARIES})
endif()

if (PNG_FOUND)
    message(STATUS "Found PNG, Building PNG Output Support")

    target_compile_definitions(ray PRIVATE RTR_PNG_SUPPORT=1)

    target_include_directories(ray PUBLIC ${PNG_INCLUDE_DIR})
    target_link_libraries(ray ${PNG_LIBRARY})
endif()

if (JPEG_FOUND)
    message(STATUS "Found JPEG, Building JPEG Texture Support")

    target_compile_definitions(ray PRIVATE RTR_JPEG_SUPPORT=1)

    target_include_directories(ray PUBLIC ${JPEG_INCLUDE_DIR})
    target_link_libraries(ray ${JPEG_LIBRARY})
endif()

if (OpenEXR_FOUND)
    message(STATUS "Found OpenEXR, Building EXR Output Support")

    target_compile_definitions(ray PRIVATE RTR_OPENEXR_SUPPORT=1)

    target_include_directories(ray PUBLIC ${OPENEXR_INCLUDE_DIRS})
    target_link_libraries(ray ${OPENEXR_LIBRARIES})
endif()

if (spdlog_FOUND)
    message(STATUS "Found Spdlog, Building logging support")

    target_compile_definitions(ray PRIVATE RTR_SPDLOG_SUPPORT=1)

    target_include_directories(ray PUBLIC ${SPDLOG_INCLUDE_DIR})
endif()

if (CURSES_FOUND)
    message(STATUS "Found Curses, Building Console UI")
    target_compile_definitions(ray PRIVATE RTR_CURSES_SUPPORT=1)
    target_include_directories(ray PUBLIC ${CURSES_INCLUDE_DIRS})
    target_link_libraries(ray ${CURSES_LIBRARIES})
endif()

if (TBB_FOUND)
    message(STATUS "Found Threading Building Blocks")
    target_compile_definitions(ray PRIVATE RTR_TBB_SUPPORT=1)
    target_include_directories(ray PUBLIC ${TBB_INCLUDE_DIRS})
    target_link_libraries(ray tbb)
endif()

#cotire(ray)

target_include_directories(tests PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(tests ${Boost_LIBRARIES})
target_include_directories(tests PUBLIC ${Boost_INCLUDE_DIRS})

target_include_directories(tests PUBLIC 3rd_party/gsl/include)
#target_include_directories(tests PUBLIC 3rd_party/glm)

target_include_directories(tests PUBLIC ${PNG_INCLUDE_DIR})
target_link_libraries(tests ${PNG_LIBRARY})

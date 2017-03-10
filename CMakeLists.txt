cmake_minimum_required(VERSION 3.6)
project(ray)
LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules/")

set(CMAKE_CXX_STANDARD 14)

set(DEBUG_FLAGS "-g -O0")
set(RELEASE_FLAGS "-Ofast -march=native -fno-rtti")

if (CMAKE_BUILD_TYPE MATCHES "Release")
    message("Performance Build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS}")
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    message("Debug Build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${DEBUG_FLAGS}")
endif()

if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
endif()

set (LIGHT_FILES lights/point_light.hpp lights/ambient_light.hpp)
set (SHAPE_FILES
        shapes/mesh.hpp shapes/mesh.cpp
        shapes/sphere.hpp shapes/sphere.cpp
        shapes/triangle.hpp shapes/triangle.cpp)
set (PHYSICS_FILES
        physics/ray.cpp physics/ray.hpp
        physics/octree.hpp
        physics/collision.hpp physics/collision.cpp
        physics/aabb.hpp
        physics/aabb_tri_intersect.cpp)

set (TinyXML_FILES 3rd_party/tinyxml2/tinyxml2.cpp)

set (COMMON_FILES ${LIGHT_FILES} ${SHAPE_FILES} ${TinyXML_FILES} ${PHYSICS_FILES}
        camera.cpp camera.hpp
        transform.hpp
        scene.hpp scene.cpp
        materials/material.hpp
        rtr_fwd.hpp
        rtr_config.hpp
        vertex.hpp
        meta_list.hpp
        xml_parse.cpp xml_parse.hpp
        utility.hpp utility.cpp
        materials/material.cpp
        lights.hpp
        shapes.hpp render_configs.hpp)

set (GIL_HDR_FILES gil_extension/exr/detail/color_space_traits.cpp)

set (TEST_FILES ${GIL_HDR_FILES} ${COMMON_FILES} tests/physics/collisions.cpp tests/tests_main.cpp)

set (SOURCE_FILES ${GIL_HDR_FILES} ${COMMON_FILES} main.cpp)

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

add_executable(ray ${SOURCE_FILES})
add_executable(tests ${TEST_FILES})

target_compile_definitions(ray PRIVATE)

find_package(Boost REQUIRED COMPONENTS system thread filesystem)
find_package(PNG REQUIRED)
find_package(OpenEXR)

target_include_directories(ray PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(ray ${Boost_LIBRARIES})
target_include_directories(ray PUBLIC ${Boost_INCLUDE_DIRS})

target_include_directories(ray PUBLIC 3rd_party/gsl/include)
target_include_directories(ray PUBLIC 3rd_party/glm)
target_include_directories(ray PUBLIC 3rd_party/tinyxml2)

target_include_directories(ray PUBLIC ${PNG_INCLUDE_DIR})
target_link_libraries(ray ${PNG_LIBRARY})

if (OpenEXR_FOUND)
    message("Found OpenEXR, Building EXR Output Support")

    target_compile_definitions(ray PRIVATE RTR_OPENEXR_SUPPORT=1)

    target_include_directories(ray PUBLIC ${OPENEXR_INCLUDE_DIRS})
    target_link_libraries(ray ${OPENEXR_LIBRARIES})
endif()

target_include_directories(tests PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(tests ${Boost_LIBRARIES})
target_include_directories(tests PUBLIC ${Boost_INCLUDE_DIRS})

target_include_directories(tests PUBLIC 3rd_party/gsl/include)
target_include_directories(tests PUBLIC 3rd_party/glm)
target_include_directories(tests PUBLIC 3rd_party/tinyxml2)

target_include_directories(tests PUBLIC ${PNG_INCLUDE_DIR})
target_link_libraries(tests ${PNG_LIBRARY})

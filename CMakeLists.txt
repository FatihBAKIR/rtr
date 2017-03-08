cmake_minimum_required(VERSION 3.6)
project(ray)

set(CMAKE_CXX_STANDARD 14)

set(RELEASE_FLAGS "-pthread -O3 -march=native -fno-exceptions -fno-rtti")

if (CMAKE_BUILD_TYPE MATCHES "Release")
    message("Performance Build")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${RELEASE_FLAGS}")
endif()

set(SOURCE_FILES main.cpp physics/ray.cpp physics/ray.hpp camera.cpp camera.hpp transform.hpp scene.hpp shapes/sphere.cpp
        shapes/sphere.hpp material.hpp rtr_fwd.hpp shapes/triangle.cpp shapes/triangle.hpp vertex.hpp physics/octree.hpp
        physics/collision.hpp physics/collision.cpp physics/aabb.hpp shapes/mesh.cpp shapes/mesh.hpp scene.cpp
        rtr_config.hpp meta_list.hpp physics/aabb_tri_intersect.cpp)

set(SOURCE_FILES ${SOURCE_FILES} 3rd_party/tinyxml2/tinyxml2.cpp xml_parse.cpp xml_parse.hpp utility.hpp utility.cpp)

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

target_compile_definitions(ray PRIVATE)

find_package(Boost REQUIRED COMPONENTS system thread filesystem)
find_package(PNG REQUIRED)

target_include_directories(ray PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(ray ${Boost_LIBRARIES})
target_include_directories(ray PUBLIC ${Boost_INCLUDE_DIRS})

target_include_directories(ray PUBLIC 3rd_party/gsl/include)
target_include_directories(ray PUBLIC 3rd_party/glm)
target_include_directories(ray PUBLIC 3rd_party/tinyxml2)

target_include_directories(ray PUBLIC ${PNG_INCLUDE_DIR})
target_link_libraries(ray ${PNG_LIBRARY})
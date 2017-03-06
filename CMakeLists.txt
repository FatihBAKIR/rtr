cmake_minimum_required(VERSION 3.6)
project(ray)

set(CMAKE_CXX_STANDARD 14)

set(RELEASE_FLAGS -pthread -O3 -march=native -fno-exceptions -fno-rtti)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(SOURCE_FILES main.cpp physics/ray.cpp physics/ray.hpp camera.cpp camera.hpp transform.hpp scene.hpp shapes/sphere.cpp
        shapes/sphere.hpp material.hpp rtr_fwd.hpp shapes/triangle.cpp shapes/triangle.hpp vertex.hpp physics/octree.hpp
        physics/collision.hpp physics/collision.cpp physics/aabb.hpp shapes/mesh.cpp shapes/mesh.hpp scene.cpp
        rtr_config.hpp meta_list.hpp physics/aabb_tri_intersect.cpp)

set(SOURCE_FILES ${SOURCE_FILES} 3rd_party/tinyxml2/tinyxml2.cpp xml_parse.cpp xml_parse.hpp)

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
cmake_minimum_required(VERSION 3.6)
project(ray)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(SOURCE_FILES main.cpp physics/ray.cpp physics/ray.hpp camera.cpp camera.hpp transform.hpp scene.hpp shapes/sphere.cpp shapes/sphere.hpp material.hpp rtr_fwd.hpp shapes/triangle.cpp shapes/triangle.hpp vertex.hpp physics/octree.hpp physics/collision.hpp physics/collision.cpp physics/aabb.hpp shapes/mesh.cpp shapes/mesh.hpp scene.cpp rtr_config.hpp meta_list.hpp physics/aabb_tri_intersect.cpp)

add_executable(ray ${SOURCE_FILES})

find_package(Boost REQUIRED COMPONENTS system thread filesystem)

target_include_directories(ray PUBLIC ${CMAKE_SOURCE_DIR})

target_link_libraries(ray ${Boost_LIBRARIES})
target_include_directories(ray PUBLIC ${Boost_INCLUDE_DIRS})

target_include_directories(ray PUBLIC 3rd_party/gsl/include)
target_include_directories(ray PUBLIC 3rd_party/glm)

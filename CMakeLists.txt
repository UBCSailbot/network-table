cmake_minimum_required(VERSION 3.10.0)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
# If you set this to ON, make sure you
# have installed ROS and catkin is in your path.
set(ENABLE_ROS OFF CACHE BOOL
    "Compile executables which require ROS?")

project(network-table CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++14 -Wall -Wextra -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(lib/core_libs.cmake)
set(ProtobufIncludePath ${CMAKE_CURRENT_BINARY_DIR}/src)

include_directories(src ${ProtobufIncludePath})
add_subdirectory(src)
add_subdirectory(projects)
add_subdirectory(test)

cmake_minimum_required(VERSION 3.5.0)
project(opengl-template VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)

include(cmake/external.cmake)

include(cmake/config.cmake)

add_subdirectory(src)
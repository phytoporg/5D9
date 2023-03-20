cmake_minimum_required(VERSION 3.9.0)

project(countdown C)

include(${PROJECT_SOURCE_DIR}/external/SDL.cmake)

add_subdirectory(src)

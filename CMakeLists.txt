cmake_minimum_required(VERSION 3.9.0)

project(countdown)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

include(${PROJECT_SOURCE_DIR}/external/SDL.cmake)

add_subdirectory(src)

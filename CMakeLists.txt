cmake_minimum_required(VERSION 3.9.0)

project(fivednine)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

include(${PROJECT_SOURCE_DIR}/external/SDL.cmake)
include(${PROJECT_SOURCE_DIR}/external/SDLImage.cmake)
include(${PROJECT_SOURCE_DIR}/external/GLM.cmake)
include(${PROJECT_SOURCE_DIR}/external/OpenGL.cmake)
include(${PROJECT_SOURCE_DIR}/external/GLEW.cmake)

add_subdirectory(src)

cmake_minimum_required (VERSION 3.9 FATAL_ERROR)

find_package(GLEW REQUIRED FATAL_ERROR)
include_directories(${GLEW_INCLUDE_DIRS})

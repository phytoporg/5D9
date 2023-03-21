cmake_minimum_required (VERSION 3.9)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/lib
    ${SDL2_INCLUDE_DIRS})

file(GLOB SOURCES 
    log/*.cpp
    render/*.cpp)
add_library(countdownlib ${SOURCES})

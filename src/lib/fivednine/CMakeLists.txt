cmake_minimum_required (VERSION 3.9)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/lib
    ${SDL2_INCLUDE_DIRS})

file(GLOB SOURCES 
    cli/*.cpp
    log/*.cpp
    render/*.cpp
    system/*.cpp)
add_library(fivedninelib ${SOURCES})

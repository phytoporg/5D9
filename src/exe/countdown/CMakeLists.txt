cmake_minimum_required(VERSION 3.9.0)

set(TARGETNAME countdown)

file(GLOB SOURCES *.cpp)
add_executable(${TARGETNAME} ${SOURCES})

cmake_minimum_required(VERSION 3.9.0)

set(TARGETNAME countdown)

include_directories(
    ${PROJECT_SOURCE_DIR}/src/lib)

file(GLOB SOURCES *.cpp)
add_executable(${TARGETNAME} ${SOURCES})

target_link_libraries(${TARGETNAME}
    countdownlib
    ${OPENGL_LIBRARIES}
    ${SDL2_LIBRARIES})

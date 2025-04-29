cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/sdk/cmake/toolchain.cmake CACHE STRING "CMake toolchain file")

project(scum-sdk)

set(SCUM_SAMPLE hello_world CACHE STRING "Sample name")

add_subdirectory(sdk/samples/${SCUM_SAMPLE})

cmake_minimum_required(VERSION 3.20)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake/toolchain.cmake CACHE STRING "CMake toolchain file")
set(SCUM_PROGRAMMER_CALIBRATE ON CACHE BOOL "Calibrate the device")

project(radio_example C)

include(../../cmake/scum-sdk.cmake)

add_scum_application(
    APPLICATION
        ${PROJECT_NAME}
    FILES
        main.c
    INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS
        gpio
        optical
        radio
)

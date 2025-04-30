cmake_minimum_required(VERSION 3.20)

project(bsp C)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/../../cmake)
include(../cmake/scum-sdk)

# ADC
list(APPEND ADC_SRCS
    adc.c
    adc.h
)
add_scum_library(TARGET adc FILES ${ADC_SRCS})

# GPIO
list(APPEND GPIO_SRCS
    gpio.c
    gpio.h
)
add_scum_library(TARGET gpio FILES ${GPIO_SRCS})

# IEEE 802.15.4
list(APPEND IEEE802154_SRCS
    ieee_802_15_4.c
    ieee_802_15_4.h
)
add_scum_library(TARGET ieee802154 FILES ${IEEE802154_SRCS})

# MATRIX
list(APPEND MATRIX_SRCS
    matrix.c
    matrix.h
)
add_scum_library(TARGET matrix FILES ${MATRIX_SRCS})

# OPTICAL
list(APPEND OPTICAL_SRCS
    optical.c
    optical.h
)
add_scum_library(TARGET optical FILES ${OPTICAL_SRCS})

# RADIO
list(APPEND RADIO_SRCS
    radio.c
    radio.h
)
add_scum_library(TARGET radio FILES ${RADIO_SRCS})

# RFTIMER
list(APPEND RFTIMER_SRCS
    rftimer.c
    rftimer.h
)
add_scum_library(TARGET rftimer FILES ${RFTIMER_SRCS})

# RING BUFFER
list(APPEND RING_BUFFER_SRCS
    ring_buffer.c
    ring_buffer.h
)
add_scum_library(TARGET ring_buffer FILES ${RING_BUFFER_SRCS})

# SCM3C HW INTERFACE
list(APPEND SCM3C_HW_INTERFACE_SRCS
    scm3c_hw_interface.c
    scm3c_hw_interface.h
)
add_scum_library(TARGET scm3c_hw_interface FILES ${SCM3C_HW_INTERFACE_SRCS})

# SPI
list(APPEND SPI_SRCS
    spi.c
    spi.h
)
add_scum_library(TARGET spi FILES ${SPI_SRCS})

# SYSTICK
list(APPEND SYSTICK_SRCS
    systick.c
    systick.h
)
add_scum_library(TARGET systick FILES ${SYSTICK_SRCS})

# INIT
list(APPEND INIT_SRCS
    startup.c
)
add_scum_library(TARGET init FILES ${INIT_SRCS})

# SYS
list(APPEND SYS_SRCS
    syscalls.c
)
add_scum_library(TARGET sys FILES ${SYS_SRCS})

# TUNING
list(APPEND TUNING_SRCS
    tuning.c
    tuning.h
)
add_scum_library(TARGET tuning FILES ${TUNING_SRCS})

# UART
list(APPEND UART_SRCS
    uart.c
    uart.h
)
add_scum_library(TARGET uart FILES ${UART_SRCS})

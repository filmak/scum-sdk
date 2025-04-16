message(STATUS "Loading toolchain file: ${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake")

# Target definition
set(CMAKE_SYSTEM_NAME  Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

set(TOOLCHAIN arm-none-eabi)
if(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
    set(EXECUTABLE_EXTENSION ".exe")
else()
    set(EXECUTABLE_EXTENSION "")
endif()
find_program(TOOLCHAIN_PREFIX_CC ${TOOLCHAIN}-gcc${EXECUTABLE_EXTENSION} PATHS /usr /usr/local /opt /opt/local /opt/homebrew REQUIRED)
cmake_path(GET TOOLCHAIN_PREFIX_CC PARENT_PATH TOOLCHAIN_PREFIX_CC_DIR)
cmake_path(GET TOOLCHAIN_PREFIX_CC_DIR PARENT_PATH TOOLCHAIN_PREFIX)
if(TOOLCHAIN_PREFIX_CC_NOTFOUND)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL Linux)
        set(TOOLCHAIN_PREFIX "/usr")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Darwin)
        set(TOOLCHAIN_PREFIX "/usr/local")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL Windows)
        message(STATUS "Please specify the TOOLCHAIN_PREFIX !\n For example: -DTOOLCHAIN_PREFIX=\"C:/Program Files/GNU Tools ARM Embedded\" ")
    else()
        set(TOOLCHAIN_PREFIX "/usr")
        message(FATAL_ERROR "No TOOLCHAIN_PREFIX specified, using default: " ${TOOLCHAIN_PREFIX})
    endif()
endif()

if(NOT EXISTS "${TOOLCHAIN_PREFIX}/bin/${TOOLCHAIN}-gcc${EXECUTABLE_EXTENSION}")
    message(FATAL_ERROR
        "Toolchain prefix not found (${TOOLCHAIN_PREFIX}/bin/${TOOLCHAIN}-gcc${EXECUTABLE_EXTENSION}). "
        "Please set TOOLCHAIN_PREFIX to the path of your toolchain or make that the toolchain gcc "
        "is in your PATH."
    )
else()
    message(STATUS "Toolchain prefix found: ${TOOLCHAIN_PREFIX}")
endif()

set(TOOLCHAIN_BIN_DIR ${TOOLCHAIN_PREFIX}/bin)
set(TOOLCHAIN_INC_DIR ${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/include)
set(TOOLCHAIN_LIB_DIR ${TOOLCHAIN_PREFIX}/${TOOLCHAIN}/lib)

#---------------------------------------------------------------------------------------
# Set compilers
#---------------------------------------------------------------------------------------
set(CMAKE_C_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${EXECUTABLE_EXTENSION} CACHE INTERNAL "C Compiler")
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-g++${EXECUTABLE_EXTENSION} CACHE INTERNAL "C++ Compiler")
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-gcc${EXECUTABLE_EXTENSION} CACHE INTERNAL "ASM Compiler")
set(CMAKE_OBJCOPY ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objcopy${EXECUTABLE_EXTENSION})
set(CMAKE_OBJDUMP ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-objdump${EXECUTABLE_EXTENSION${EXECUTABLE_EXTENSION}})
set(CMAKE_SIZE ${TOOLCHAIN_BIN_DIR}/${TOOLCHAIN}-size${EXECUTABLE_EXTENSION})

set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_PREFIX}/${TOOLCHAIN} ${CMAKE_PREFIX_PATH})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Perform compiler test with static library
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

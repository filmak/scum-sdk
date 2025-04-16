
set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/../../bsp/scum.ld CACHE STRING "Linker script")

if(NOT CMAKE_BUILD_TYPE)
    set(SCUM_BUILD_TYPE "MinSizeRel")
else()
    set(SCUM_BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()
string(TOUPPER "CMAKE_C_FLAGS_${SCUM_BUILD_TYPE}" SCUM_BUILD_TYPE_FLAGS)

# Object build options
if(NOT DEFINED SCUM_BUILD_C_FLAGS)
string(APPEND SCUM_BUILD_C_FLAGS
    ${${SCUM_BUILD_TYPE_FLAGS}}
    " -mcpu=cortex-m0"
    " -mthumb"
    " -std=c17"
    " -fdebug-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}/../..="
    " -pedantic"
    " -Wall"
    " -Werror"
    " -Wstrict-prototypes"
    " -fdata-sections"
    " -ffunction-sections"
    " -fomit-frame-pointer"
    " -fshort-wchar"
)
endif()

if(NOT DEFINED SCUM_BUILD_C_FLAGS)
    string(APPEND SCUM_BUILD_CXX_FLAGS
        ${SCUM_BUILD_C_FLAGS}
        " -std=c++11"
    )
endif()

if(NOT DEFINED SCUM_BUILD_ASM_FLAGS)
    string(APPEND SCUM_BUILD_ASM_FLAGS
        ${SCUM_BUILD_C_FLAGS}
        " -x assembler-with-cpp"
    )
endif()

# Linker flags
if(NOT DEFINED SCUM_BUILD_LD_FLAGS)
    string(APPEND SCUM_BUILD_LD_FLAGS
        " -specs=nano.specs"
        " -Wl,--gc-sections"
        " -Wl,--print-memory-usage"
        " -Wl,--no-wchar-size-warning"
        " -T${LINKER_SCRIPT}"
        " -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map"
    )
endif()

set(CMAKE_C_FLAGS ${SCUM_BUILD_C_FLAGS} CACHE INTERNAL "C Compiler options")
set(CMAKE_CXX_FLAGS ${SCUM_BUILD_CXX_FLAGS} CACHE INTERNAL "C++ Compiler options")
set(CMAKE_ASM_FLAGS ${SCUM_BUILD_ASM_FLAGS} CACHE INTERNAL "ASM Compiler options")
set(CMAKE_APPLICATION_LINKER_FLAGS ${SCUM_BUILD_LD_FLAGS} CACHE INTERNAL "Linker options")

cmake_path(GET CMAKE_CURRENT_LIST_DIR PARENT_PATH SCUM_SDK_BASE_DIR)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Include build and linker flags
include(scum-build)

find_program(
    SCUM_PROGRAMMER
    NAMES
        scum-programmer
        main.py
    HINTS
        ${SCUM_SDK_BASE_DIR}/../scum_programmer
)

function(add_scum_library)
    set(target TARGET)
    set(files FILES)
    cmake_parse_arguments(PARSE_ARGV 0 arg "" "${target}" "${files}")
    add_library(${arg_TARGET} OBJECT EXCLUDE_FROM_ALL ${arg_FILES})
    set_property(TARGET ${arg_TARGET} PROPERTY C_STANDARD 17)
    target_include_directories(${arg_TARGET} PRIVATE
        ${SCUM_SDK_BASE_DIR}/bsp/cmsis
        ${SCUM_SDK_BASE_DIR}/bsp
    )
endfunction()

function(add_scum_application)
    set(app_single APPLICATION)
    set(app_multi FILES INCLUDES DEPENDS)
    cmake_parse_arguments(PARSE_ARGV 0 arg "" "${app_single}" "${app_multi}")

    set(TARGET ${arg_APPLICATION}.elf CACHE INTERNAL "APPLICATION")

    add_subdirectory(${SCUM_SDK_BASE_DIR}/bsp libs)
    add_executable(${TARGET}
        ${arg_FILES}
        ${SCUM_SDK_BASE_DIR}/bsp/helpers.h
        ${SCUM_SDK_BASE_DIR}/bsp/scum.h
    )

    set_property(TARGET ${TARGET} PROPERTY C_STANDARD 17)

    set(CORE_DEPS
        init
        scm3c_hw_interface
        sys
        uart
    )

    set(APP_DEPS
        ${CORE_DEPS}
        ${arg_DEPENDS}
    )

    foreach(dep ${APP_DEPS})
        string(TOUPPER "MODULE_${dep}" _module_define)
        list(APPEND _compile_defines "${_module_define}")
    endforeach()

    foreach(dep ${APP_DEPS})
        set_target_properties(${dep} PROPERTIES COMPILE_DEFINITIONS "${_compile_defines}")
    endforeach()
    set_target_properties(${TARGET} PROPERTIES COMPILE_DEFINITIONS "${_compile_defines}")

    target_include_directories(${TARGET} PRIVATE
        ${arg_INCLUDES}
        ${SCUM_SDK_BASE_DIR}/bsp/cmsis
        ${SCUM_SDK_BASE_DIR}/bsp
    )
    target_link_libraries(${TARGET} PRIVATE ${APP_DEPS})

    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex ${TARGET} ${arg_APPLICATION}.hex
    )

    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Obinary ${TARGET} ${arg_APPLICATION}.bin
    )

    add_custom_command(
        TARGET ${TARGET}
        POST_BUILD
        COMMAND ${CMAKE_SIZE} ${CMAKE_CURRENT_BINARY_DIR}/${TARGET}
    )

    if(NOT SCUM_PROGRAMMER_NOTFOUND)

        set(SCUM_PROGRAMMER_PORT "" CACHE STRING "Port to use for SCuM programmer")
        if(SCUM_PROGRAMMER_PORT)
            set(SCUM_PROGRAMMER_PORT_OPTION -p ${SCUM_PROGRAMMER_PORT})
        else()
            set(SCUM_PROGRAMMER_PORT_OPTION "")
        endif()

        set(SCUM_PROGRAMMER_CALIBRATE CACHE BOOL "Calibrate the device")
        if(DEFINED SCUM_PROGRAMMER_CALIBRATE AND SCUM_PROGRAMMER_CALIBRATE)
            set(SCUM_PROGRAMMER_CALIBRATE_OPTION --calibrate)
        else()
            set(SCUM_PROGRAMMER_CALIBRATE_OPTION "")
        endif()

        add_custom_target(
            load
            COMMAND ${SCUM_PROGRAMMER} ${SCUM_PROGRAMMER_PORT_OPTION} ${SCUM_PROGRAMMER_CALIBRATE_OPTION} ${CMAKE_CURRENT_BINARY_DIR}/${arg_APPLICATION}.bin
            DEPENDS ${TARGET}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            USES_TERMINAL
            COMMENT "Loading ${CMAKE_CURRENT_BINARY_DIR}/${arg_APPLICATION}.bin"
        )
    endif()
endfunction()

cmake_minimum_required(VERSION 3.22)

find_program(OPENOCD_EXE openocd)
find_program(CMAKE_GDB ${TOOLCHAIN_PREFIX}gdb HINTS ${TOOLCHAIN_PATH})

set(MODULE_NAME UTILS_${PROJECT_NAME})
set(CONFIGURE_INPUT_FILES_PATH
    ${CMAKE_SOURCE_DIR}/Cmake/Utils/configure-files
)
set(VSC_CONFIGURE_OUTPUT_FILES_PATH
    ${CMAKE_BINARY_DIR}/configure-files/vscode
)

set(VSC_WORK_FOLDER_PROVIDE "\${workspaceFolder}")
set(VSC_EXECUTABLE_PROVIDE "\${command:cmake.launchTargetPath}")

if(NOT OPENOCD_EXE OR NOT CMAKE_GDB)
    message(WARNING "Could not find `openocd` or '${TOOLCHAIN_PREFIX}gdb' in PATH")
else()
    find_file(TOOLCHAIN_FILE Arm-none-eabi-toolchain.cmake ${CMAKE_SOURCE_DIR}/Cmake/Toolchain)
    find_file(VSC_LAUNCH_FILE launch.cfg ${CONFIGURE_INPUT_FILES_PATH})
    find_file(VSC_CPP_PROPERTIES_FILE c_cpp_properties.cfg ${CONFIGURE_INPUT_FILES_PATH})
    find_file(VSC_ESTENSIONS_FILE extensions.cfg ${CONFIGURE_INPUT_FILES_PATH})
    find_file(VSC_SETTINGS_FILE settings.cfg ${CONFIGURE_INPUT_FILES_PATH})
    find_file(CUSTOM_ENV_FILE custom_env.cfg ${CONFIGURE_INPUT_FILES_PATH})

    if(
        NOT TOOLCHAIN_FILE OR
        NOT VSC_LAUNCH_FILE OR
        NOT VSC_CPP_PROPERTIES_FILE OR
        NOT VSC_ESTENSIONS_FILE OR
        NOT VSC_SETTINGS_FILE OR
        NOT CUSTOM_ENV_FILE
    )
        message(WARNING "Could not find *.cfg file in PATH::${CONFIGURE_INPUT_FILES_PATH}")
    else()
        file(READ ${CUSTOM_ENV_FILE} CUSTOM_ENV_CONTENT)

        string(REPLACE "\n" ";" CUSTOM_ENV_CONTENT "${CUSTOM_ENV_CONTENT}")

        foreach(line IN LISTS CUSTOM_ENV_CONTENT)
            if(NOT line STREQUAL "")
                string(REGEX MATCH "^(.*)=(.*)$" match ${line})

                if(match)
                    set(key "${CMAKE_MATCH_1}")
                    set(value "${CMAKE_MATCH_2}")

                    set(${key} "${value}")
                endif()
            endif()
        endforeach()

        configure_file(${VSC_LAUNCH_FILE} ${VSC_CONFIGURE_OUTPUT_FILES_PATH}/launch.json)
        configure_file(${VSC_CPP_PROPERTIES_FILE} ${VSC_CONFIGURE_OUTPUT_FILES_PATH}/c_cpp_properties.json)
        configure_file(${VSC_ESTENSIONS_FILE} ${VSC_CONFIGURE_OUTPUT_FILES_PATH}/extensions.json)
        configure_file(${VSC_SETTINGS_FILE} ${VSC_CONFIGURE_OUTPUT_FILES_PATH}/settings.json)

        add_custom_target(
            ${MODULE_NAME}_VSC_CONFIGURE
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${VSC_CONFIGURE_OUTPUT_FILES_PATH}/launch.json
            ${CMAKE_SOURCE_DIR}/.vscode/launch.json

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${VSC_CONFIGURE_OUTPUT_FILES_PATH}/c_cpp_properties.json
            ${CMAKE_SOURCE_DIR}/.vscode/c_cpp_properties.json

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${VSC_CONFIGURE_OUTPUT_FILES_PATH}/extensions.json
            ${CMAKE_SOURCE_DIR}/.vscode/extensions.json

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${VSC_CONFIGURE_OUTPUT_FILES_PATH}/settings.json
            ${CMAKE_SOURCE_DIR}/.vscode/settings.json
            COMMENT "Moving .json files to .vscode folder"
            VERBATIM
        )
    endif()
endif()

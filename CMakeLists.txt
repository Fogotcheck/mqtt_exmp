cmake_minimum_required(VERSION 3.22)

project(mqtt VERSION 0.1.0 LANGUAGES CXX C ASM)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")
set(CPACK_PACKAGE_CHECKSUM MD5)
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

add_compile_definitions(
    "configRECORD_STACK_HIGH_ADDRESS=1"
)

include(FetchContent)
include(CPack)
include(Cmake/Opts/Custom_build_opts.cmake)
include(Cmake/Utils/pre-commit.cmake)
include(Cmake/Utils/vscode-config.cmake)
include(Cmake/Utils/change_log.cmake)

add_subdirectory(Chip)
add_subdirectory(Lib)
add_subdirectory(AppMain)

target_install_binary(${PROJECT_NAME})

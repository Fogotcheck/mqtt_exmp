cmake_minimum_required(VERSION 3.22)

add_library(
    Logger
    src/Logger.c
)

target_include_directories(
    Logger
    PUBLIC
    inc
)

target_link_libraries(
    Logger
    PRIVATE
    ${PROJECT_NAME}_CHIP_INTERFACE
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(
        Logger
        PUBLIC
        DEBUG_LOG=3
    )
else()
    target_compile_definitions(
        Logger
        PUBLIC
        DEBUG_LOG=1
    )
endif()

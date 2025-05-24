cmake_minimum_required(VERSION 3.22)

set(MODULE_NAME ${PROJECT_NAME})

add_executable(
    ${MODULE_NAME}
    AppMain.cpp
)

target_add_core_source(${MODULE_NAME})

target_include_directories(
    ${MODULE_NAME}
    PRIVATE
    inc
)

target_link_libraries(
    ${MODULE_NAME}
    ${PROJECT_NAME}_CHIP_INTERFACE
    ${PROJECT_NAME}_LIB_INTERFACE
)

target_post_build(${MODULE_NAME})

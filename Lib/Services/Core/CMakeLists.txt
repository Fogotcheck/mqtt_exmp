cmake_minimum_required(VERSION 3.22)

add_library(
    CoreService
    src/CoreService.cpp
    hw/src/CoreServiceHW.cpp
)

target_include_directories(
    CoreService
    PUBLIC
    inc
    hw/inc
)

target_link_libraries(
    CoreService
    Services
    freertos_kernel
)

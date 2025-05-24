cmake_minimum_required(VERSION 3.22)

set(FREERTOS_HEAP "4" CACHE STRING "" FORCE)
set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)

add_library(freertos_config INTERFACE)

target_include_directories(
    freertos_config
    SYSTEM INTERFACE
    FreeRTOS_config
    FreeRTOS_custom
)

target_sources(
    freertos_config
    INTERFACE
    FreeRTOS_custom/cmsis_os2.c
)

target_link_libraries(
    freertos_config
    INTERFACE
    ${PROJECT_NAME}_CHIP_INTERFACE
)

FetchContent_Declare(freertos_kernel
    GIT_REPOSITORY https://github.com/FreeRTOS/FreeRTOS-Kernel.git
    GIT_TAG main
)

FetchContent_MakeAvailable(freertos_kernel)

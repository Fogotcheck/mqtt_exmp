cmake_minimum_required(VERSION 3.22)

add_library(${PROJECT_NAME}_CHIP_INTERFACE INTERFACE)

add_subdirectory(STM32F439ZITx)

target_compile_options(
    ${PROJECT_NAME}_CHIP_INTERFACE
    INTERFACE
    -T${MCU_DEFAULT_LINKER_SCRIPT}
)

target_link_options(
    ${PROJECT_NAME}_CHIP_INTERFACE
    INTERFACE
    -T${MCU_DEFAULT_LINKER_SCRIPT}
)

target_link_libraries(
    ${PROJECT_NAME}_CHIP_INTERFACE
    INTERFACE
    STM32F439ZITx_HAL
)

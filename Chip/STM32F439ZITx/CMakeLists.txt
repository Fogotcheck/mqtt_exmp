cmake_minimum_required(VERSION 3.22)

set(MODULE_NAME STM32F439ZITx_HAL)

set(MCPU "-mcpu=cortex-m4" CACHE STRING "" FORCE)
set(MFPU "-mfpu=fpv4-sp-d16" CACHE STRING "" FORCE)
set(MFLOAT_ABI "-mfloat-abi=hard" CACHE STRING "" FORCE)
set(MCU_DEVICE "STM32F439xx" CACHE STRING "" FORCE)
set(MCU_DEFAULT_LINKER_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/STM32F439ZITX_FLASH.ld"
    CACHE STRING "" FORCE
)

add_library(${MODULE_NAME}_INTERFACE INTERFACE)

target_compile_options(
    ${MODULE_NAME}_INTERFACE
    INTERFACE
    ${MCPU}
    ${MFPU}
    ${MFLOAT_ABI}
)

target_link_options(
    ${MODULE_NAME}_INTERFACE
    INTERFACE
    ${MCPU}
    ${MFPU}
    ${MFLOAT_ABI}
)

target_compile_definitions(
    ${MODULE_NAME}_INTERFACE
    INTERFACE
    ${MCU_DEVICE}
    ${MCPU_CORE}
    "USE_HAL_DRIVER"
)

target_include_directories(
    ${MODULE_NAME}_INTERFACE
    INTERFACE
    Core/Inc
    Drivers/CMSIS/Device/ST/STM32F4xx/Include
    Drivers/CMSIS/Include
    Drivers/STM32F4xx_HAL_Driver/Inc
    Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
)

file(GLOB_RECURSE ${MODULE_NAME}_HAL_SRC
    Drivers/STM32F4xx_HAL_Driver/Src/*.c
)

function(target_add_core_source TARGET_NAME)
    file(GLOB_RECURSE STM32F439ZITx_HAL_SYSTEM_SRC
        ${CMAKE_SOURCE_DIR}/Chip/STM32F439ZITx/Core/Src/*.c
    )

    target_sources(
        ${TARGET_NAME}
        PRIVATE
        ${STM32F439ZITx_HAL_SYSTEM_SRC}
        ${CMAKE_SOURCE_DIR}/Chip/STM32F439ZITx/Core/Startup/startup_stm32f439zitx.s
    )
endfunction(target_add_core_source TARGET_NAME)

add_library(
    ${MODULE_NAME}
    ${${MODULE_NAME}_HAL_SRC}
)

target_link_libraries(
    ${MODULE_NAME}
    ${MODULE_NAME}_INTERFACE
)

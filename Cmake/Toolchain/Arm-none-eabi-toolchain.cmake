cmake_minimum_required(VERSION 3.22)

set(CMAKE_SYSTEM_PROCESSOR "arm" CACHE STRING "")
set(CMAKE_SYSTEM_NAME "Generic" CACHE STRING "")

# Specify toolchain. NOTE When building from inside STM32CubeIDE the location of the toolchain is resolved by the "MCU Toolchain" project setting (via PATH).
set(TOOLCHAIN_PREFIX "arm-none-eabi-")

find_program(CMAKE_C_COMPILER "${TOOLCHAIN_PREFIX}gcc" HINTS ${TOOLCHAIN_PATH})
find_program(CMAKE_ASM_COMPILER "${TOOLCHAIN_PREFIX}gcc" HINTS ${TOOLCHAIN_PATH})
find_program(CMAKE_CXX_COMPILER "${TOOLCHAIN_PREFIX}g++" HINTS ${TOOLCHAIN_PATH})

find_program(CMAKE_AR "${TOOLCHAIN_PREFIX}ar" HINTS ${TOOLCHAIN_PATH})
find_program(CMAKE_LINKER "${TOOLCHAIN_PREFIX}ld" HINTS ${TOOLCHAIN_PATH})
find_program(CMAKE_OBJCOPY "${TOOLCHAIN_PREFIX}objcopy" HINTS ${TOOLCHAIN_PATH})
find_program(CMAKE_RANLIB "${TOOLCHAIN_PREFIX}ranlib" HINTS ${TOOLCHAIN_PATH})
find_program(CMAKE_STRIP "${TOOLCHAIN_PREFIX}ld" HINTS ${TOOLCHAIN_PATH})

set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")

add_compile_options(
  -fdata-sections
  -ffunction-sections
  -Wall
)

add_link_options(
  --specs=nosys.specs
  -Wl,-Map=output.map,--cref,--print-memory-usage
  -Wl,--gc-sections
  -static
  -Wl,--start-group
  -lc
  -lm
  -Wl,--end-group
)

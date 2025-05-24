cmake_minimum_required(VERSION 3.22)

FetchContent_Declare(lwip_kernel
    GIT_REPOSITORY https://github.com/lwip-tcpip/lwip.git
    GIT_TAG master
)

FetchContent_MakeAvailable(lwip_kernel)

set(LWIP_DIR ${lwip_kernel_SOURCE_DIR})
set(LWIP_INCLUDE_DIRS
    "${LWIP_DIR}/src/include"
    "${LWIP_DIR}/contrib"
)

add_library(
    lwip
    target/ethernetif.c
    target/phy/lan8742.c
    app/lwip.c
)

add_library(
    lwip_interface
    INTERFACE
)

target_compile_definitions(
    lwip_interface
    INTERFACE
    LWIP_DEBUG=1
)


target_sources(
    lwip_interface
    INTERFACE
    target/ports/freertos/sys_arch.c
)

target_include_directories(
    lwip_interface
    SYSTEM INTERFACE
    opts
    app
    target
    target/phy
    target/ports/freertos
    target/ports/freertos/include
    ${LWIP_INCLUDE_DIRS}
)

target_link_libraries(
    lwipcore
    lwip_interface
    freertos_kernel
)

target_link_libraries(
    lwipallapps
    lwip_interface
    freertos_kernel
)

target_link_libraries(
    lwipmbedtls
    lwip_interface
    freertos_kernel
)

target_link_libraries(
    lwip
    lwipcore
    lwipallapps
    lwipmbedtls
)

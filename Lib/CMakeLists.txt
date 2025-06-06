cmake_minimum_required(VERSION 3.22)

add_library(${PROJECT_NAME}_LIB_INTERFACE INTERFACE)

add_subdirectory(FreeRTOS)
add_subdirectory(Lwip)
add_subdirectory(Network)
add_subdirectory(Logger)
add_subdirectory(Services)


target_link_libraries(
    ${PROJECT_NAME}_LIB_INTERFACE
    INTERFACE
    freertos_kernel
    Network
    Services
    UserServices
    Logger
)

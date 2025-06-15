cmake_minimum_required(VERSION 3.22)

add_library(${PROJECT_NAME}_SERVICES_INTERFACE INTERFACE)

add_subdirectory(Base)

target_link_libraries(
    Services
    ${PROJECT_NAME}_CHIP_INTERFACE
)

add_subdirectory(Core)

target_link_libraries(
    ${PROJECT_NAME}_SERVICES_INTERFACE
    INTERFACE
    Services
    CoreService
)

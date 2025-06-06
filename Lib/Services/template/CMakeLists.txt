cmake_minimum_required(VERSION 3.22)

add_library(UserServices STATIC)

target_sources(
    UserServices
    PRIVATE
    src/UserService.cpp
)

set_target_properties(
    UserServices
    PROPERTIES
    VERSION_MAJOR 0xff
    VERSION_MINOR 0xaa
    VERSION_PATCH 0xbb
)

target_include_directories(
    UserServices
    PUBLIC
    inc
)

target_compile_definitions(
    UserServices
    PUBLIC
    USER_SERVICES_VERSION_MAJOR=$<TARGET_PROPERTY:UserServices,VERSION_MAJOR>
    USER_SERVICES_VERSION_MINOR=$<TARGET_PROPERTY:UserServices,VERSION_MINOR>
    USER_SERVICES_VERSION_PATCH=$<TARGET_PROPERTY:UserServices,VERSION_PATCH>
)

target_link_libraries(
    UserServices
    PRIVATE
    Services
)

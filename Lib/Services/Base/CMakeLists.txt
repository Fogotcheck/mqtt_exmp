cmake_minimum_required(VERSION 3.22)

FetchContent_Declare(
    lwjson
    GIT_REPOSITORY https://github.com/MaJerle/lwjson.git
    GIT_TAG main
)

FetchContent_MakeAvailable(lwjson)

add_library(
    Services
    src/Services.cpp
)

target_include_directories(
    Services
    PUBLIC
    inc
)

set_target_properties(
    Services
    PROPERTIES
    VERSION_MAJOR 0
    VERSION_MINOR 1
    VERSION_PATCH 0
)

target_compile_definitions(
    Services
    PUBLIC
    SERVICES_VERSION_MAJOR=$<TARGET_PROPERTY:Services,VERSION_MAJOR>
    SERVICES_VERSION_MINOR=$<TARGET_PROPERTY:Services,VERSION_MINOR>
    SERVICES_VERSION_PATCH=$<TARGET_PROPERTY:Services,VERSION_PATCH>
)

target_compile_features(
    Services
    PUBLIC
    cxx_std_20
)

target_link_libraries(
    Services
    lwjson
)

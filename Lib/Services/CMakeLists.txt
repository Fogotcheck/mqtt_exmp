cmake_minimum_required(VERSION 3.22)

FetchContent_Declare(Services
    GIT_REPOSITORY git@github.com:Fogotcheck/Services.git
    GIT_TAG master
)

FetchContent_MakeAvailable(Services)

target_link_libraries(
    Services
    ${PROJECT_NAME}_LIB_INTERFACE
)

add_subdirectory(template)

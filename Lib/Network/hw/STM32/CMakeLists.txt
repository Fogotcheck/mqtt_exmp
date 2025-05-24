cmake_minimum_required(VERSION 3.22)

add_library(HW_stmNetwork)

target_sources(
    HW_stmNetwork
    PRIVATE
    HW_StmNetwork.cpp
)

target_include_directories(
    HW_stmNetwork
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
    HW_stmNetwork
    lwip
    Network
)

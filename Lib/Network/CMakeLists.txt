cmake_minimum_required(VERSION 3.22)

add_library(
    Network
    src/Network.cpp
    src/mqttClient.cpp
)

target_include_directories(
    Network
    PUBLIC
    inc
)

target_link_libraries(
    Network
    lwip
)

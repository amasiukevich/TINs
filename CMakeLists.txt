cmake_minimum_required(VERSION 3.0)

project(aaa-protocol)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -pedantic")

include_directories(src)

file(GLOB SERVER_FILES
    src/server/main_server.cpp
    src/server/server.cpp
)

file(GLOB PROXY_FILES
    src/proxy/main_proxy.cpp
    src/proxy/proxy.cpp
)

file(GLOB CLIENT_FILES
    src/client/main_client.cpp
    src/client/client.cpp
)

file(GLOB COMMON_FILES
)

add_executable(server ${SERVER_FILES} ${COMMON_FILES})
add_executable(proxy ${PROXY_FILES} ${COMMON_FILES})
add_executable(client ${CLIENT_FILES} ${COMMON_FILES})

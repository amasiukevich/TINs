cmake_minimum_required(VERSION 3.0)

project(aaa-protocol)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -pedantic")

include_directories(
    src
    lib
    ${Boost_INCLUDE_DIRS}
)

file(GLOB SERVER_FILES
    src/server/main_server.cpp
    src/server/server.cpp
    src/server/http.cpp
)

file(GLOB PROXY_FILES
    src/proxy/connection_manager.cpp
    src/proxy/main_proxy.cpp
    src/proxy/proxy.cpp
)

file(GLOB CLIENT_FILES
    src/client/main_client.cpp
    src/client/client.cpp
)

file(GLOB COMMON_FILES
    src/utility.cpp
    src/aaa.cpp
)

find_package(Boost 1.71.0 COMPONENTS system thread REQUIRED)

add_executable(server ${SERVER_FILES} ${COMMON_FILES})
target_link_libraries(server pthread ${Boost_LIBRARIES})

add_executable(proxy ${PROXY_FILES} ${COMMON_FILES})
add_executable(client ${CLIENT_FILES} ${COMMON_FILES})

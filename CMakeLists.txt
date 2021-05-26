cmake_minimum_required(VERSION 3.0)

project(aaa-protocol)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -pedantic")

include_directories(
    src
    lib
    ${Boost_INCLUDE_DIRS}
)

file(GLOB DEVICE_FILES
    src/device/*.cpp
)

file(GLOB PROXY_FILES
    src/proxy/*.cpp
)

file(GLOB CLIENT_FILES
    src/client/*.cpp
)

file(GLOB COMMON_FILES
    src/common/*.cpp
)

find_package(Boost 1.71.0 COMPONENTS system thread REQUIRED)

add_executable(device ${DEVICE_FILES} ${COMMON_FILES})
target_link_libraries(device pthread ${Boost_LIBRARIES})

add_executable(proxy ${PROXY_FILES} ${COMMON_FILES})
target_link_libraries(proxy pthread ${Boost_LIBRARIES})

add_executable(client ${CLIENT_FILES} ${COMMON_FILES})
target_link_libraries(client pthread ${Boost_LIBRARIES})

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)

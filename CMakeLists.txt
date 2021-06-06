cmake_minimum_required(VERSION 3.0)

project(aaa-protocol)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -pedantic")

add_custom_target(copy_config ALL COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/config" "${CMAKE_BINARY_DIR}/config")

# Libraries
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.71.0 COMPONENTS system thread REQUIRED)
add_subdirectory(lib/googletest)

# Headers

include_directories(
    src
    lib
    ${Boost_INCLUDE_DIRS}
    lib/rapidjson/include
    lib/googletest/googletest/include
    lib/spdlog/include
)

# Source files

file(GLOB DEVICE_FILES
    src/device/*.cpp
)

file(GLOB PROXY_FILES
    src/proxy/*.cpp
)

file(GLOB COMMON_FILES
    src/common/*.cpp
    src/common/*.h
)

# Programs

add_executable(device ${DEVICE_FILES} ${COMMON_FILES})
target_link_libraries(device pthread ${Boost_LIBRARIES})

add_executable(proxy ${PROXY_FILES} ${COMMON_FILES} src/proxy/exceptions.h)
target_link_libraries(proxy pthread ${Boost_LIBRARIES})

# Tests 

enable_testing()

add_executable(test1 test/test.cpp ${COMMON_FILES})
target_link_libraries(test1 pthread ${Boost_LIBRARIES} gtest)
add_test(test1 test1)

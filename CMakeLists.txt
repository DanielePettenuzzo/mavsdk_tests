cmake_minimum_required(VERSION 3.11)

project(mavsdk_tests)

find_package(MAVSDK REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-std=c++14 -Wall -Wextra -Werror ")

include(FindPkgConfig)

include_directories(${CMAKE_SOURCE_DIR})
include_directories("/usr/local/include/mavsdk/plugins/mavlink_passthrough")
include_directories("/usr/local/include/mavsdk")

add_executable(send_autopilot_version
send_autopilot_version.cpp
)

target_link_libraries(send_autopilot_version
    MAVSDK::mavsdk
    MAVSDK::mavsdk_info
    MAVSDK::mavsdk_telemetry
    MAVSDK::mavsdk_mavlink_passthrough
    pthread
    SegFault
)
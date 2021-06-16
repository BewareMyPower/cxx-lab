cmake_minimum_required(VERSION 3.1)
project(asio-demo)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

add_definitions("-DASIO_STANDALONE")

find_package(Boost REQUIRED)
message(STATUS "Boost version: ${Boost_VERSION}")

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
include_directories(${Boost_INCLUDE_DIRS})

add_executable(daytime_server daytime_server.cc)
add_executable(daytime_client daytime_client.cc)
add_executable(periodic_task periodic_task.cc)

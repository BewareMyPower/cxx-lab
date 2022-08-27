cmake_minimum_required(VERSION 3.1)
project(asio-demo)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

add_definitions("-DASIO_STANDALONE")

find_package(Boost REQUIRED)
message(STATUS "Boost version: ${Boost_VERSION}")

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
)

add_executable(daytime_server src/daytime_server.cc)
add_executable(daytime_client src/daytime_client.cc)
add_executable(periodic_task src/periodic_task.cc)
add_executable(connect_timeout src/connect_timeout.cc)
add_executable(multi_threads_run src/multi_threads_run.cc)

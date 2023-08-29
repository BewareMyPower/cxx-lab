cmake_minimum_required(VERSION 3.1)
project(asio-demo)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

find_package(Boost REQUIRED)
message(STATUS "Boost version: ${Boost_VERSION}")

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
)

find_package(Threads REQUIRED)

add_executable(daytime_server src/daytime_server.cc)
target_link_libraries(daytime_server PRIVATE ${CMAKE_THREAD_LIBS_INIT})
add_executable(daytime_client src/daytime_client.cc)
target_link_libraries(daytime_client PRIVATE ${CMAKE_THREAD_LIBS_INIT})
add_executable(periodic_task src/periodic_task.cc)
target_link_libraries(periodic_task PRIVATE ${CMAKE_THREAD_LIBS_INIT})
add_executable(connect_timeout src/connect_timeout.cc)
target_link_libraries(connect_timeout PRIVATE ${CMAKE_THREAD_LIBS_INIT})
add_executable(multi_threads_run src/multi_threads_run.cc)
target_link_libraries(multi_threads_run PRIVATE ${CMAKE_THREAD_LIBS_INIT})
add_executable(async_write_server src/async_write_server.cc)
target_link_libraries(async_write_server PRIVATE ${CMAKE_THREAD_LIBS_INIT})
add_executable(async_write_client src/async_write_client.cc)
target_link_libraries(async_write_client PRIVATE ${CMAKE_THREAD_LIBS_INIT})
add_executable(thread_pool src/thread_pool.cc)
target_link_libraries(thread_pool PRIVATE ${CMAKE_THREAD_LIBS_INIT})

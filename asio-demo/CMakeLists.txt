cmake_minimum_required(VERSION 3.5)
project(asio-demo)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        add_compile_definitions(_LIBCPP_ENABLE_ASSERTIONS)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_definitions(_GLIBCXX_ASSERTIONS)
    endif()
endif ()

include(FetchContent)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG 1f8d154829b902dbc45a651587c6c6df948358e8 # asio 1.28.1
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/thirdparty
)
FetchContent_MakeAvailable(asio)

find_package(Boost REQUIRED)
message(STATUS "Boost version: ${Boost_VERSION}")

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${Boost_INCLUDE_DIRS}
    ${asio_SOURCE_DIR}/asio/include
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
add_executable(tcp_e2e src/tcp_e2e.cc)
target_link_libraries(tcp_e2e PRIVATE ${CMAKE_THREAD_LIBS_INIT})

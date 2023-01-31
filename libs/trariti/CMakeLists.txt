cmake_minimum_required(VERSION 3.5)

project (Trariti CXX)

add_library(TraritiLib INTERFACE)
target_include_directories(TraritiLib INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

find_package(Threads REQUIRED)

add_executable(TraritiQueueTest ${CMAKE_CURRENT_SOURCE_DIR}/tests/TraritiQueueTest.cpp)

target_link_libraries(TraritiQueueTest TraritiLib Threads::Threads)

enable_testing()
add_test(TraritiQueueTest TraritiQueueTest)

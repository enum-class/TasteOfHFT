cmake_minimum_required(VERSION 3.5)

project (BroadcastQueue CXX)

add_library(BroadcastQueue INTERFACE)
target_include_directories(BroadcastQueue INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>)

find_package(Threads REQUIRED)

add_executable(BroadcastQueueTest ${CMAKE_CURRENT_SOURCE_DIR}/tests/BroadcastQueueTest.cpp)
target_link_libraries(BroadcastQueueTest BroadcastQueue Threads::Threads)

enable_testing()
add_test(BroadcastQueueTest BroadcastQueueTest)

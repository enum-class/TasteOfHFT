cmake_minimum_required(VERSION 3.5)

project (HFT CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/conan.cmake)
conan_cmake_configure(REQUIRES gtest/cci.20210126
                      GENERATORS cmake_find_package)

conan_cmake_autodetect(settings)
conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings})

add_subdirectory(libs)

add_library(hftlib
    src/Lib.cpp
)

target_include_directories(hftlib
    PUBLIC
    include
)

enable_testing()

find_package(GTest)

add_executable(${PROJECT_NAME} tests/LibTest.cpp)

target_link_libraries(${PROJECT_NAME}
    hftlib
    GTest::gtest
    GTest::gtest_main
)

add_test(test_all ${PROJECT_NAME})

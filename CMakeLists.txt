cmake_minimum_required(VERSION 3.10)

project(eseed_window)

set(CMAKE_CXX_STANDARD 17)
set(FLAGS)

# Library

add_library(eseed_window)
target_include_directories(eseed_window PUBLIC include)

if(WIN32)
    target_sources(eseed_window PRIVATE src/win32/window.cpp)
endif()

# Testing

add_executable(eseed_window_test_basic test/basic.cpp)
target_link_libraries(eseed_window_test_basic eseed_window)
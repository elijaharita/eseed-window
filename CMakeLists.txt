cmake_minimum_required(VERSION 3.10)

project(eseed_window)

option(ESD_WND_BUILD_EXAMPLES OFF)
option(ESD_WND_ENABLE_VULKAN_SUPPORT OFF)

set(CMAKE_CXX_STANDARD 17)

# Library

add_library(eseed_window)
target_include_directories(eseed_window PUBLIC include)

# Include vulkan if requested

if(ESD_WND_ENABLE_VULKAN_SUPPORT)
    find_package(Vulkan REQUIRED)
    target_link_libraries(eseed_window Vulkan::Vulkan)
    target_compile_definitions(eseed_window PRIVATE ESD_WND_INCLUDE_VULKAN_HPP)
endif()

# Choose platform specific implementation

if(WIN32)
    message("Using Win32")
    target_sources(eseed_window PRIVATE src/win32/window.cpp)
    if(ESD_WND_ENABLE_VULKAN_SUPPORT)
        target_sources(eseed_window PRIVATE src/win32/vulkanwindow.cpp)
    endif()
endif()

if(UNIX AND NOT APPLE)
    message("Using X11")
    target_sources(eseed_window PRIVATE src/x11/window.cpp)
    find_package(X11 REQUIRED)
    target_link_libraries(eseed_window ${X11_LIBRARIES})
    if(ESD_WND_ENABLE_VULKAN_SUPPORT)
        target_sources(eseed_window PRIVATE src/x11/vulkanwindow.cpp)
    endif()
endif()

# Testing

if(ESD_WND_BUILD_EXAMPLES)

    # Basic Input
    add_subdirectory(examples/basicinput)

    # Sizing
    add_subdirectory(examples/dimensions)

    if(ESD_WND_ENABLE_VULKAN_SUPPORT)

        # Vulkan Support
        add_subdirectory(examples/vulkan)
        
    endif()
    
endif()
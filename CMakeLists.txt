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
    target_sources(eseed_window PRIVATE src/win32/window.cpp)
    if(ESD_WND_ENABLE_VULKAN_SUPPORT)
        target_sources(eseed_window PRIVATE src/win32/vulkanwindow.cpp)
    endif()
endif()

# Testing

if(ESD_WND_BUILD_EXAMPLES)

    # Basic Test
    add_subdirectory(examples/basicinput)

    if(ESD_WND_ENABLE_VULKAN_SUPPORT)

        # Vulkan Test
        add_subdirectory(examples/vulkan)
        
    endif()
    
endif()
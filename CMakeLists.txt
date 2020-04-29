cmake_minimum_required(VERSION 3.10)

project(eseed_window)

option(ESD_WND_BUILD_EXAMPLES OFF "Build Examples")
option(ESD_WND_ENABLE_VULKAN_SUPPORT OFF "Enable Vulkan Support")

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

# Auto detect platform if not manually set

if(NOT ESD_WND_PLATFORM)
    if(WIN32)
        set(PLATFORM "Win32")
    elseif(APPLE)
        set(PLATFORM "Cocoa")
    elseif(UNIX) # Unix and not Apple
        set(PLATFORM "X11")
    else()
        message(FATAL_ERROR "Could not auto detect platform, it may be unimplemented")
    endif()

    set(ESD_WND_PLATFORM "${PLATFORM}" CACHE STRING "Target Platform" FORCE)
endif()

# Choose platform specific implementation

if(ESD_WND_PLATFORM STREQUAL "Win32")

    message("Using Win32")
    target_sources(eseed_window PRIVATE src/win32/window.cpp)
    if(ESD_WND_ENABLE_VULKAN_SUPPORT)
        target_sources(eseed_window PRIVATE src/win32/vulkanwindow.cpp)
    endif()

elseif(ESD_WND_PLATFORM STREQUAL "Cocoa")

    message("Using Cocoa")
    target_sources(eseed_window PRIVATE src/cocoa/window.cpp)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    target_link_libraries(eseed_window ${COCOA_LIBRARY})

elseif(ESD_WND_PLATFORM STREQUAL "X11")

    message("Using X11")
    target_sources(eseed_window PRIVATE src/x11/window.cpp)
    find_package(X11 REQUIRED)
    target_link_libraries(eseed_window ${X11_LIBRARIES})
    if(ESD_WND_ENABLE_VULKAN_SUPPORT)
        target_sources(eseed_window PRIVATE src/x11/vulkanwindow.cpp)
    endif()
    
else()

    message(FATAL_ERROR "Unknown platform ${ESD_WND_PLATFORM}, it may have been typed incorrectly or is unimplemented")

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
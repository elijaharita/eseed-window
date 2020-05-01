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
    elseif(UNIX AND NOT APPLE)
        set(PLATFORM "X11")
    else()
        message(FATAL_ERROR "Could not auto detect platform, it may be unimplemented")
    endif()

    set(ESD_WND_PLATFORM ${PLATFORM} CACHE STRING "Target Platform" FORCE)

endif()

string(TOLOWER ${ESD_WND_PLATFORM} PLATFORM_DIR_NAME)

# Choose platform specific implementation

message("Using ${ESD_WND_PLATFORM}")

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/platforms/${PLATFORM_DIR_NAME}")
    message(FATAL_ERROR "Unkown platform, it may have been typed incorrectly or is unimplemented")
endif()

add_subdirectory("platforms/${PLATFORM_DIR_NAME}")

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
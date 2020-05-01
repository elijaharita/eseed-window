cmake_minimum_required(VERSION 3.10)

target_sources(eseed_window PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/window.cpp")
if(ESD_WND_ENABLE_VULKAN_SUPPORT)
    target_sources(eseed_window PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/vulkanwindow.cpp")
endif()
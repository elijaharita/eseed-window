// Copyright (c) 2020 Elijah Seed Arita
//
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal 
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in 
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.

#pragma once

#include <eseed/window/window.hpp>

#ifdef ESD_WND_INCLUDE_VULKAN_HPP
#include <vulkan/vulkan.hpp>
#endif

#ifdef ESD_WND_INCLUDE_VULKAN_H
#include <vulkan/vulkan.h>
#endif

namespace esd::wnd {

class VulkanWindow : public Window {
public:
    VulkanWindow(std::string title, WindowSize size) : Window(title, size) {}

    // Get instance extensions required to create a Vulkan surface on the
    // current platform    
    std::vector<const char*> getRequiredSurfaceInstanceExtensions() const;

// Only if the C++ bindings are included
// (defined in vulkan/vulkan.hpp header)
#ifdef VULKAN_HPP
    vk::SurfaceKHR createSurface(const vk::Instance& instance) const;
#endif

    VkSurfaceKHR createSurface(const VkInstance& instance) const;
};

}
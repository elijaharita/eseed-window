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

#ifndef VK_USE_PLATFORM_XLIB_KHR
#define VK_USE_PLATFORM_XLIB_KHR
#endif

#include "impl.hpp"
#include <eseed/window/vulkanwindow.hpp>

using namespace esd::wnd;

std::vector<const char*> VulkanWindow::getRequiredSurfaceInstanceExtensions() const {
    return {
        VK_KHR_SURFACE_EXTENSION_NAME,
        VK_KHR_XLIB_SURFACE_EXTENSION_NAME
    };
}

#ifdef VULKAN_HPP
vk::SurfaceKHR VulkanWindow::createSurface(const vk::Instance& instance) const {
    return instance.createXlibSurfaceKHR(vk::XlibSurfaceCreateInfoKHR()
        .setDpy(impl->display)
        .setWindow(impl->window)
    );
}
#endif

VkSurfaceKHR VulkanWindow::createSurface(const VkInstance& instance) const {
    VkXlibSurfaceCreateInfoKHR ci = {};
    ci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    ci.dpy = impl->display;
    ci.window = impl->window;

    VkSurfaceKHR surface;
    vkCreateXlibSurfaceKHR(instance, &ci, nullptr, &surface);

    return surface;
}
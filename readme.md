# eseed window
`namespace esd::wnd`

## Goal
A cross-platform, minimal, modern window library for C++17 supporting a variety of rendering APIs (such as Vulkan, OpenGL, etc.)

## A message from me
Hello everyone. This project was originally designed as a part of my game engine. However, I think that sharing work is pretty cool, and it's going to be a lot of work to create a cross-platform project like this by myself. So, if anyone is interested in contributing a different platform / new render API support, I'd love to work with you! In any case, I'll slowly be adding functionality myself, but it'll take a lot of research.

## Features
- Current supported platforms
  - Win32
- Current supported rendering APIs
  - Vulkan
- Input handling
  - Polling only atm
  - Keyboard
    - Key callback
      - Applicable key codes are mapped to their ASCII analogs
    - Char input callback
- Title management
- Size and position management
- Fullscreen
- Currently NOT thread safe

### Planned
- More platform support
  - Mac, Wayland, X, etc. (needs a lot more research on my part, contributions appreciated)
  - More Windows support
    - IME input
- More rendering API support
  - OpenGL context management
  - Possibly others
- Ability to wait on events in addition to polling
- Input state getters (in addition to callbacks)
- More input handling
  - Mouse
  - Controllers / gamepads
  - Additional (needs more research)
- Thread safety
- Sound? (possible)
- Anything else as it comes along :)

## Ghetto temporary documentation

### Basics
Currently, the only supported rendering API is Vulkan, and the only supported platform is Win32. When more platforms are added, effective usage should be exactly the same and completely platform-independent.

Windows are created with a simple constructor taking a title and size. This may be expanded with additional optional parameters in the future.

After the window has been created, it needs to be polled for events continuously.

When the window class goes out of scope or is destroyed, the window itself will be freed and destroyed automatically.

```cpp
#include <esd/window/window.hpp>

int main() {
  
    // Window creation

    esd::wnd::Window window("Title", { 1366, 768 });

    // Event polling until window closes
    
    while (!window.isCloseRequested()) {
        window.poll();
    }
}
```

### Input handling
Window input can be intercepted by setting the listeners in the window object.

#### Raw Keyboard Input
```cpp
window.keyHandler = [](esd::wnd::KeyEvent e) { ... }
```

Called any time a key is pressed or released.

`keyHandler` is a function taking `esd::wnd::KeyEvent`. `KeyEvent` contains two members -- an `esd::window::KeyCode` enum, and a boolean `down`, with true indicating that a key was pressed, and false indicating that a key was released. 

#### Keyboard Char Input
```cpp
window.keyCharHandler = [](char32_t c) { ... }
```

`keyCharHandler` is a function taking `char32_t` 

### Vulkan support
The `esd::wnd::VulkanWindow` class is a helper class extending the base window class to provide platform-specific Vulkan functionality (surface creation). Both the C Vulkan library and C++ bindings (`vulkan.hpp`) are supported.

To use `VulkanWindow`, the CMake build files must be generated with `-DESD_WND_ENABLE_VULKAN_SUPPORT`.

Make sure to include the Vulkan header before `<esd/window/vulkanwindow.hpp>`.

`VulkanWindow` provides 2 additional functions:
- `std::vector<const char*> getRequiredSurfaceInstanceExtensions()`
- `VkSurface createSurface(const VkInstance& instance)`
- `vk::Surface createSurface(const vk::Instance& instance)` (only if `vulkan.hpp` is included)

```cpp
#include <vulkan/vulkan.hpp>
#include <esd/window/vulkanwindow.hpp>

int main() {
    // Window creation
    esd::wnd::VulkanWindow window("Title" { 1366, 768 });
    
    // Get required instance extensions for surface creation
    std::vector<const char*> extensions = 
        window.getRequiredSurfaceInstanceExtensions();
    
    // Vulkan instance creation
    vk::InstanceCreateInfo instanceCreateInfo;
    ...
    instanceCreateInfo.setEnabledExtensionCount((uint32_t)extensions.size());
    instanceCreateInfo.setPpEnabledExtensions(extensions.data());
    ...
    vk::Instance instance = vk::createInstance(instanceCreateInfo);

    // Vulkan surface creation with eseed window
    vk::SurfaceKHR surface = window.createSurface(instance);

    ...

    while (!window.isCloseRequested()) {

        ...
      
        window.poll();
    }

    ...
}
```
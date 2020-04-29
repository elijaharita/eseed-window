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

#include <eseed/window/window.hpp>
#include "impl.hpp"
#include <iostream>
#include <vector>

using namespace esd::wnd;

Window::Window(
    std::string title, 
    WindowSize size, 
    std::optional<WindowPos> pos
) { 
    impl = std::make_unique<Impl>();

    auto NSAutoreleasePool = objc_getClass("NSAutoreleasePool");
    auto NSApplication = objc_getClass("NSApplication");
    auto NSWindow = objc_getClass("NSWindow");
    auto NSApp = objc_getClass("NSApp");

    impl->pool = reinterpret_cast<id(*)(Class, SEL)>(objc_msgSend)(
        NSAutoreleasePool, 
        sel_registerName("alloc")
    );
    if (!impl->pool)
        throw std::runtime_error("Could not create pool");
    reinterpret_cast<void(*)(id, SEL)>(objc_msgSend)(
        impl->pool, 
        sel_registerName("init")
    );

    impl->app = reinterpret_cast<id(*)(Class, SEL)>(objc_msgSend)(
        NSApplication, 
        sel_registerName("sharedApplication")
    );
    if (!impl->app)
        throw std::runtime_error("Could not create app");

    impl->window = reinterpret_cast<id(*)(Class, SEL)>(objc_msgSend)(
        NSWindow,
        sel_registerName("alloc")
    );
    if (!impl->window)
        throw std::runtime_error("Could not create window");
    reinterpret_cast<void(*)(id, SEL, CGRect, int, int, BOOL)>(objc_msgSend)(
        impl->window, 
        sel_registerName("initWithContentRect:styleMask:backing:defer:"), 
        CGRectMake(0, 0, 100, 100), 
        0x3, 
        2, 
        NO
    );
    reinterpret_cast<void(*)(id, SEL)>(objc_msgSend)(
        impl->window,
        sel_registerName("center")
    );
    reinterpret_cast<void(*)(id, SEL, Ivar)>(objc_msgSend)(
        impl->window,
        sel_registerName("makeKeyAndOrderFront:"),
        class_getClassVariable(NSApp, "mainWindow")
    );

    reinterpret_cast<void(*)(id, SEL)>(objc_msgSend)(
        impl->app,
        sel_registerName("run")
    );

    std::cout << "pool: " << impl->pool << std::endl;
}

Window::~Window() {
    reinterpret_cast<void(*)(id, SEL)>(objc_msgSend)(
        impl->pool,
        sel_registerName("release")
    );
}

void Window::close() {
    
}

void Window::poll() {
    
}

void Window::waitEvents() {
    
}

std::string Window::getTitle() {
    return "";
}

void Window::setTitle(std::string title) {
    
}

WindowSize Window::getSize() {
    return {};
}

void Window::setSize(WindowSize size) {
    
}

WindowPos Window::getPos() {
    return {};
}

void Window::setPos(WindowPos pos) {
    
}

bool Window::isCloseRequested() {
    return false;
}

void Window::setCloseRequested(bool closeRequested) {
    
}

bool Window::isFullscreen() {
    return false;
}

void Window::setFullscreen(bool fullscreen) {
    
}

bool Window::isKeyDown(Key keyCode) {
    return false;
}

bool Window::isKeyToggled(Key key) {
    return false;
}

CursorPos Window::getCursorPos() {
    return {};
}

void Window::setCursorPos(CursorPos pos) {
    
}

CursorPos Window::getCursorScreenPos() {
    return {};
}

void Window::setCursorScreenPos(CursorPos pos) {
    
}

bool Window::isMouseButtonDown(MouseButton button) {
    return false;
}
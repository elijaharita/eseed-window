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

#include <CoreFoundation/CoreFoundation.h>
#include <objc/objc.h>
#include <objc/objc-runtime.h>
#include <iostream>

using namespace esd::wnd;

Window::Window(
    std::string title, 
    WindowSize size, 
    std::optional<WindowPos> pos
) {
    id app = nullptr;
    id pool = reinterpret_cast<id>(objc_getClass("NSAutoreleasePool"));

    if (!pool) std::cout << "Could not create pool" << std::endl;
}

Window::~Window() {
    
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
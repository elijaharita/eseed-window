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

#include "impl.hpp"
#include <eseed/window/window.hpp>
#include <X11/Xlib.h>
#include <iostream>

using namespace esd::wnd;

esd::wnd::Window::Window(std::string title, WindowSize size, std::optional<WindowPos> pos) {
    impl = std::make_unique<Impl>();
    impl->display = XOpenDisplay(nullptr);

    if (impl->display == nullptr) {
        throw std::runtime_error("Could not open X11 display");
    }

    impl->screen = DefaultScreen(impl->display);
    
    impl->window = XCreateSimpleWindow(
        impl->display, 
        RootWindow(impl->display, impl->screen), 
        pos ? pos->x : 0,
        pos ? pos->y : 0,
        size.w, 
        size.h, 
        1, 
        BlackPixel(impl->display, impl->screen), 
        WhitePixel(impl->display, impl->screen)
    );
    XSelectInput(impl->display, impl->window, ExposureMask | KeyPressMask);
    XMapWindow(impl->display, impl->window);

    // Set protocols to intercept

    impl->WM_DELETE_WINDOW = XInternAtom(impl->display, "WM_DELETE_WINDOW", False);

    std::vector<Atom> protocols = {
        impl->WM_DELETE_WINDOW
    };
    
    XSetWMProtocols(impl->display, impl->window, protocols.data(), protocols.size());
}

esd::wnd::Window::~Window() {
    XCloseDisplay(impl->display);
}

void esd::wnd::Window::poll() {
    // Get events as long as there is at least one available
    while (XPending(impl->display)) {
        XEvent event;

        // Whether or not to forward this message back to the window manager
        // Set to false in order to disable forwarding
        bool propogateEvent = true;
        
        XNextEvent(impl->display, &event);

        std::cout << "event" << std::endl;

        switch (event.type) {
        case ClientMessage:
            {
                if ((Atom)event.xclient.data.l[0] == impl->WM_DELETE_WINDOW) {
                    impl->closeRequested = true;
                    propogateEvent = false;
                }
            }
            break;
        }
    }
}

void esd::wnd::Window::waitEvents() {
    // Wait for an event to become available
    XEvent event;
    XPeekEvent(impl->display, &event);

    // Then handle all the queued events like normal
    poll();
}

std::string esd::wnd::Window::getTitle() {
    return "";
}

void esd::wnd::Window::setTitle(std::string title) {

}

WindowSize esd::wnd::Window::getSize() {
    return {};
}

void esd::wnd::Window::setSize(WindowSize size) {

}

WindowPos esd::wnd::Window::getPos() {
    return {};
}

void esd::wnd::Window::setPos(WindowPos pos) {

}

bool esd::wnd::Window::isCloseRequested() {
    return impl->closeRequested;
}

void esd::wnd::Window::setCloseRequested(bool closeRequested) {
    impl->closeRequested = closeRequested;
}

bool esd::wnd::Window::isFullscreen() {
    return false;
}

void esd::wnd::Window::setFullscreen(bool fullscreen) {

}

bool esd::wnd::Window::isKeyDown(Key key) {
    return false;
}

bool esd::wnd::Window::isKeyToggled(Key key) {
    return false;
}

CursorPos esd::wnd::Window::getCursorPos() {
    return {};
}

void esd::wnd::Window::setCursorPos(CursorPos pos) {

}

CursorPos esd::wnd::Window::getCursorScreenPos() {
    return {};
}

void esd::wnd::Window::setCursorScreenPos(CursorPos pos) {

}

bool esd::wnd::Window::isMouseButtonDown(MouseButton button) {
    return false;
}
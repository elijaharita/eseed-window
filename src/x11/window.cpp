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
#include "inputmappings.hpp"
#include <eseed/window/window.hpp>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <iostream>
#include <cstring>

using namespace esd::wnd;

esd::wnd::Window::Window(std::string title, WindowSize size, std::optional<WindowPos> pos) {
    impl = std::make_unique<Impl>();
    impl->display = XOpenDisplay(nullptr);

    if (impl->display == nullptr) {
        throw std::runtime_error("Could not open X11 display");
    }

    impl->screen = DefaultScreen(impl->display);
    impl->root = RootWindow(impl->display, impl->screen);
    
    impl->window = XCreateSimpleWindow(
        impl->display, 
        impl->root, 
        pos ? pos->x : 0,
        pos ? pos->y : 0,
        size.w, 
        size.h, 
        1, 
        BlackPixel(impl->display, impl->screen), 
        WhitePixel(impl->display, impl->screen)
    );

    impl->initKeyTables();
    
    XSelectInput(
        impl->display, 
        impl->window, 
        ExposureMask 
            | KeyPressMask 
            | KeyReleaseMask
            | StructureNotifyMask
    );
    XMapWindow(impl->display, impl->window);

    // Collect atoms

    impl->WM_DELETE_WINDOW = XInternAtom(impl->display, "WM_DELETE_WINDOW", False);
    impl->_NET_WM_NAME = XInternAtom(impl->display, "_NET_WM_NAME", False);
    impl->UTF8_STRING = XInternAtom(impl->display, "UTF8_STRING", False);

    // Set protocols to intercept

    std::vector<Atom> protocols = {
        impl->WM_DELETE_WINDOW // Override window close
    };
    
    XSetWMProtocols(impl->display, impl->window, protocols.data(), protocols.size());

    setTitle(title);
}

esd::wnd::Window::~Window() {
    XCloseDisplay(impl->display);
}

void esd::wnd::Window::poll() {
    // Get events as long as there is at least one available
    while (XPending(impl->display)) {
        XEvent xe;
        XNextEvent(impl->display, &xe);

        switch (xe.type) {
        case KeyPress:
        case KeyRelease:
            if (keyHandler) {
                KeyEvent event;
                event.down = xe.type == KeyPress;
                event.key = impl->fromX11KeyCode(xe.xkey.keycode);
                std::cout << "X11 Key Code: " << xe.xkey.keycode << std::endl;
                keyHandler(event);
            }
        case ClientMessage:
            {
                if ((Atom)xe.xclient.data.l[0] == impl->WM_DELETE_WINDOW) {
                    impl->closeRequested = true;
                }
            }
            break;
        case ConfigureNotify:
            // TODO: resize events
            break;
        }
    }
}

void esd::wnd::Window::waitEvents() {
    // Wait for an event to become available
    XEvent xe;
    XPeekEvent(impl->display, &xe);

    // Then handle all the queued events like normal
    poll();
}

// TODO: Avoid unnecessary allocation
std::string esd::wnd::Window::getTitle() {

    Atom actualType;
    int actualFormat;
    unsigned long nItems;
    unsigned long bytesAfter;
    unsigned char* prop;

    XGetWindowProperty(
        impl->display, 
        impl->window,
        impl->_NET_WM_NAME,
        0UL,
        0UL,
        False,
        impl->UTF8_STRING,
        &actualType,
        &actualFormat,
        &nItems,
        &bytesAfter,
        &prop
    );

    XFree((void*)prop);

    XGetWindowProperty(
        impl->display, 
        impl->window,
        impl->_NET_WM_NAME,
        0UL,
        (bytesAfter + 3UL) / 4UL, // Divide by four and round up
        False,
        impl->UTF8_STRING,
        &actualType,
        &actualFormat,
        &nItems,
        &bytesAfter,
        &prop
    );
    
    std::string title((char*)prop);

    XFree((void*)prop);
    
    return title;
}

void esd::wnd::Window::setTitle(std::string title) {
    XChangeProperty(
        impl->display, 
        impl->window, 
        impl->_NET_WM_NAME, 
        impl->UTF8_STRING,
        8, 
        PropModeReplace, 
        (unsigned char*)title.c_str(), 
        title.length()
    );
}

WindowSize esd::wnd::Window::getSize() {
    XWindowAttributes xwa;
    XGetWindowAttributes(impl->display, impl->window, &xwa);
    return { xwa.width, xwa.height };
}

void esd::wnd::Window::setSize(WindowSize size) {
    XResizeWindow(impl->display, impl->window, size.w, size.h);
}

WindowPos esd::wnd::Window::getPos() {
    int x, y;
    ::Window child;
    XTranslateCoordinates(impl->display, impl->window, impl->root, 0, 0, &x, &y, &child);

    XWindowAttributes xwa;
    XGetWindowAttributes(impl->display, impl->window, &xwa);

    return { x, y };
}

void esd::wnd::Window::setPos(WindowPos pos) {
    XMoveWindow(impl->display, impl->window, pos.x, pos.y);
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

Key esd::wnd::Window::Impl::fromX11KeyCode(unsigned int x11KeyCode) {
    if (x11KeyCode > (unsigned int)Key::LastKey)
        return Key::Unknown;

    return esdKeyTable[x11KeyCode];
}

void esd::wnd::Window::Impl::initKeyTables() {

    // Get list of XKB names and their associated key codes for the current
    // environment
    XkbDescPtr desc = XkbGetMap(display, 0, XkbUseCoreKbd);
    XkbGetNames(display, XkbKeyNamesMask, desc);

    // Size key lookup tables to fit all the possible keys
    esdKeyTable.resize(desc->max_key_code + 1);
    x11KeyTable.resize(desc->max_key_code + 1);

    for (unsigned int x11KeyCode = 0; x11KeyCode <= desc->max_key_code; x11KeyCode++) {
        
        bool found = false;
        for (auto it : keyMappings) {
            if (strncmp(it.first, desc->names->keys[x11KeyCode].name, XkbKeyNameLength) == 0) {

                esdKeyTable[x11KeyCode] = it.second;
                x11KeyTable[(std::size_t)it.second] = x11KeyCode;
                
                found = true;
                break;
            }
        }   
        
        // Any character with no corresponding esd key will be set to unknown
        if (!found) esdKeyTable[x11KeyCode] = Key::Unknown;
    }

    // Clean up desc and its key name list
    XkbFreeNames(desc, XkbKeyNamesMask, True);
    XkbFreeKeyboard(desc, 0, True);
}
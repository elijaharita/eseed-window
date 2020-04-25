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

    XSetLocaleModifiers("");
    impl->im = XOpenIM(impl->display, nullptr, nullptr, nullptr);

    if (!impl->im) {
        XSetLocaleModifiers("@im=none");
        impl->im = XOpenIM(impl->display, nullptr, nullptr, nullptr);    
    }

    impl->ic = XCreateIC(
        impl->im,
        XNInputStyle,
        XIMPreeditNothing | XIMStatusNothing,
        XNClientWindow,
        impl->window,
        XNFocusWindow,
        impl->window,
        nullptr
    );
    
    XSelectInput(
        impl->display, 
        impl->window, 
        ExposureMask 
            | KeyPressMask
            | KeyReleaseMask
            | StructureNotifyMask
            | PointerMotionMask
            | LeaveWindowMask
            | ButtonPressMask
            | ButtonReleaseMask
            | ButtonMotionMask
    );
    XMapWindow(impl->display, impl->window);

    // Collect atoms

    impl->WM_DELETE_WINDOW = XInternAtom(impl->display, "WM_DELETE_WINDOW", False);
    impl->_NET_WM_NAME = XInternAtom(impl->display, "_NET_WM_NAME", False);
    impl->_NET_WM_STATE_FULLSCREEN = XInternAtom(impl->display, "_NET_WM_STATE_FULLSCREEN", False);
    impl->_NET_FRAME_EXTENTS = XInternAtom(impl->display, "_NET_FRAME_EXTENTS", False);
    impl->_NET_WM_STATE = XInternAtom(impl->display, "_NET_WM_STATE", False);
    impl->UTF8_STRING = XInternAtom(impl->display, "UTF8_STRING", False);

    // Set protocols to intercept

    std::vector<Atom> protocols = {
        impl->WM_DELETE_WINDOW // Override window close
    };
    
    XSetWMProtocols(impl->display, impl->window, protocols.data(), protocols.size());

    setTitle(title);
}

esd::wnd::Window::~Window() {
    if (impl->display != nullptr) close();
}

void esd::wnd::Window::close() {
    XFree(impl->ic);
    XFree(impl->im);
    XDestroyWindow(impl->display, impl->window);
    XCloseDisplay(impl->display);
    impl->display = nullptr;
}

void esd::wnd::Window::poll() {
    // Get events as long as there is at least one available
    while (XPending(impl->display)) {
        XEvent xe;
        XNextEvent(impl->display, &xe);

        switch (xe.type) {
        case KeyPress:
            if (keyCharHandler) {
                if (!XFilterEvent(&xe, None)) {
                    Status status;
                    KeySym keySym;
                    char utf8[4];
                    Xutf8LookupString(impl->ic, &xe.xkey, utf8, 4, &keySym, &status);

                    KeyCharEvent event = {};

                    // Convert UTF-8 to code point
                    if ((utf8[0] & 0x80) == 0x00) {
                        event.codePoint = utf8[0];
                    } else if ((utf8[0] & 0xE0) == 0xC0) {
                        event.codePoint 
                            = (utf8[0] & 0x1F) << 6
                            | (utf8[1] & 0x3F);
                    } else if ((utf8[2] & 0xF0) == 0xE0) {
                        event.codePoint 
                            = (utf8[0] & 0x0F) << 12
                            | (utf8[1] & 0x3F) << 6
                            | (utf8[2] & 0x3F);
                    } else if ((utf8[0] & 0xF8) == 0xF0) {
                        event.codePoint 
                            = (utf8[0] & 0x07) << 18
                            | (utf8[1] & 0x3F) << 12
                            | (utf8[2] & 0x3F) << 6
                            | (utf8[3] & 0x3F);
                    }

                    if (event.codePoint != 0)
                        keyCharHandler(event);
                }
            }
            // no break
        case KeyRelease:
            if (keyHandler) {
                KeyEvent event;
                event.down = xe.type == KeyPress;
                event.key = impl->fromX11KeyCode(xe.xkey.keycode);
                keyHandler(event);
            }
            break;
        case ButtonPress:
            // Scroll wheel event (press only)
            if (xe.xbutton.button == Button4 || xe.xbutton.button == Button5) {
                constexpr double delta = 1.0;
                if (scrollHandler) {
                    ScrollEvent event = {};
                    if (xe.xbutton.button == Button4) event.vScroll = delta;
                    else event.vScroll = -delta;
                    scrollHandler(event);
                }
                break;
            }
            // no break
        case ButtonRelease:
            if (mouseButtonHandler) {
                MouseButtonEvent event;
                event.down = xe.type == ButtonPress;
                switch (xe.xbutton.button) {
                case Button1:
                    event.button = MouseButton::LButton;
                    break;
                case Button2:
                    event.button = MouseButton::MButton;
                    break;
                case Button3:
                    event.button = MouseButton::RButton;
                    break;
                default:
                    event.button = MouseButton::Unknown;
                }

                if (event.button != MouseButton::Unknown)
                    mouseButtonHandler(event);
            }
            break;
        case MotionNotify:
            if (cursorMoveHandler) {
                CursorMoveEvent event;
                event.pos = { 
                    (double)xe.xmotion.x, 
                    (double)xe.xmotion.y 
                };
                event.screenPos = { 
                    (double)xe.xmotion.x_root, 
                    (double)xe.xmotion.y_root 
                };

                // The cursor has entered the window if it was previously out
                event.entered = !impl->cursorInWindow;
                
                cursorMoveHandler(event);
            }
            impl->cursorInWindow = true;
            break;
        case LeaveNotify:
            if (cursorExitHandler) {
                impl->cursorInWindow = false;
                cursorExitHandler({});
            }
        case ClientMessage:
            {
                if ((Atom)xe.xclient.data.l[0] == impl->WM_DELETE_WINDOW) {
                    impl->closeRequested = true;
                }
            }
            break;
        case ConfigureNotify:
            // Position changed
            if (
                moveHandler && (
                    impl->lastConfigure.x != xe.xconfigure.x ||
                    impl->lastConfigure.y != xe.xconfigure.y
                )
            ) {
                MoveEvent event;
                event.pos = { xe.xconfigure.x, xe.xconfigure.y };
                moveHandler(event);
            }

            // Size changed
            if (
                resizeHandler && (
                    impl->lastConfigure.width != xe.xconfigure.width ||
                    impl->lastConfigure.height != xe.xconfigure.height
                )
            ) {
                ResizeEvent event;
                event.size = { xe.xconfigure.width, xe.xconfigure.height };
                resizeHandler(event);
            }

            impl->lastConfigure = xe.xconfigure;
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
        AnyPropertyType,
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
        AnyPropertyType,
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

    Atom actualType;
    int actualFormat;
    unsigned long nitems;
    unsigned long bytesAfter;
    unsigned long* prop;
    
    XGetWindowProperty(
        impl->display,
        impl->window,
        impl->_NET_FRAME_EXTENTS,
        0,
        sizeof(unsigned long) * 4,
        False,
        AnyPropertyType,
        &actualType,
        &actualFormat,
        &nitems,
        &bytesAfter,
        (unsigned char**)&prop
    );
    
    XMoveWindow(impl->display, impl->window, pos.x - prop[0], pos.y - prop[2]);

    XFree(prop);
}

bool esd::wnd::Window::isCloseRequested() {
    return impl->closeRequested;
}

void esd::wnd::Window::setCloseRequested(bool closeRequested) {
    impl->closeRequested = closeRequested;
}

// Iterate through all the atoms contained in the _NET_WM_STATE window property
// and check if _NET_WM_STATE_FULLSCREEN is present, to find out whether
// fullscreen mode is active
bool esd::wnd::Window::isFullscreen() {

    Atom actualType;
    int actualFormat;
    unsigned long nitems;
    unsigned long bytesAfter;
    Atom* prop;

    int propertyIndex = 0;
    
    // Iterate over each atom for _NET_WM_STATE
    while (bytesAfter > 0) {
        XGetWindowProperty(
            impl->display,
            impl->window,
            impl->_NET_WM_STATE,
            propertyIndex * sizeof(Atom),
            sizeof(Atom),
            False,
            AnyPropertyType,
            &actualType,
            &actualFormat,
            &nitems,
            &bytesAfter,
            (unsigned char**)&prop
        );

        if (*prop == impl->_NET_WM_STATE_FULLSCREEN) {
            // If _NET_WM_STATE_FULLSCREEN is found, fullscreen mode is active
            // Make sure prop is freed before returning
            XFree(prop);
            return true;
        }

        XFree(prop);

        // Check next property in the next iteration
        propertyIndex++;
    }

    // If _NET_WM_STATE_FULLSCREEN wasn't present, return false
    return false;
}

// Send a ClientMessage event to X requesting fullscreen
void esd::wnd::Window::setFullscreen(bool fullscreen) {

    XEvent xe = {};
    xe.xclient.type = ClientMessage;
    xe.xclient.display = impl->display;
    xe.xclient.window = impl->window;
    xe.xclient.message_type = impl->_NET_WM_STATE;
    xe.xclient.serial = 0;
    xe.xclient.send_event = True; // The client sent this message
    xe.xclient.format = 32;
    xe.xclient.data.l[0] = fullscreen ? impl->_NET_WM_STATE_ADD : impl->_NET_WM_STATE_REMOVE; 
    xe.xclient.data.l[1] = impl->_NET_WM_STATE_FULLSCREEN; // Property
    xe.xclient.data.l[3] = 1; // Source: 1 (Regular application sent this message)
    
    XSendEvent(
        impl->display,
        DefaultRootWindow(impl->display),
        False,
        SubstructureNotifyMask,
        &xe
    );
}

bool esd::wnd::Window::isKeyDown(Key key) {
    char keys[32];
    XQueryKeymap(impl->display, keys);

    for (size_t i = 0; i < 32; i++) {
        if (keys[i] == impl->x11KeyTable[(std::size_t)key]) return true;
        else if (i == 0) break;
    }
    
    return false;
}

bool esd::wnd::Window::isKeyToggled(Key key) {
    unsigned int state;
    XkbGetIndicatorState(impl->display, XkbUseCoreKbd, &state);

    if (key == Key::CapsLock) return state & 0x01;
    if (key == Key::NumLock) return state & 0x02;
    if (key == Key::ScrollLock) return state & 0x04;

    return false;
}

CursorPos esd::wnd::Window::getCursorPos() {

    ::Window child, root;
    int rootX, rootY;
    int winX, winY;
    unsigned int mask;
    
    XQueryPointer(
        impl->display, 
        impl->window, 
        &child,
        &root,
        &rootX,
        &rootY,
        &winX,
        &winY,
        &mask
    );

    return { (double)winX, (double)winY };
}

void esd::wnd::Window::setCursorPos(CursorPos pos) {
    XWarpPointer(
        impl->display,
        None,
        impl->window,
        0,
        0,
        0,
        0,
        (int)pos.x,
        (int)pos.y
    );
}

CursorPos esd::wnd::Window::getCursorScreenPos() {
    ::Window child, root;
    int rootX, rootY;
    int winX, winY;
    unsigned int mask;
    
    XQueryPointer(
        impl->display, 
        impl->window, 
        &child,
        &root,
        &rootX,
        &rootY,
        &winX,
        &winY,
        &mask
    );

    return { (double)rootX, (double)rootY };
}

void esd::wnd::Window::setCursorScreenPos(CursorPos pos) {
    XWarpPointer(
        impl->display,
        None,
        DefaultRootWindow(impl->display),
        0,
        0,
        0,
        0,
        (int)pos.x,
        (int)pos.y
    );
}

bool esd::wnd::Window::isMouseButtonDown(MouseButton button) {
    ::Window child, root;
    int rootX, rootY;
    int winX, winY;
    unsigned int mask;
    
    XQueryPointer(
        impl->display, 
        impl->window, 
        &child,
        &root,
        &rootX,
        &rootY,
        &winX,
        &winY,
        &mask
    );

    if (button == MouseButton::LButton) return mask & Button1Mask;
    if (button == MouseButton::MButton) return mask & Button2Mask;
    if (button == MouseButton::RButton) return mask & Button3Mask;

    // TODO: detect mouse button 4 & 5
    if (button == MouseButton::XButton1) return false;
    if (button == MouseButton::XButton2) return false;

    throw std::runtime_error("Unknown mouse button");
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
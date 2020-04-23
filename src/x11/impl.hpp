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
#include <X11/Xlib.h>

class esd::wnd::Window::Impl {
public:
    Display* display;
    int screen;
    ::Window root;
    ::Window window;
    bool closeRequested;
    Atom WM_DELETE_WINDOW;
    Atom _NET_WM_NAME;
    Atom _NET_WM_STATE_FULLSCREEN;
    Atom _NET_FRAME_EXTENTS;
    Atom _NET_MOVERESIZE_WINDOW;
    Atom _NET_WM_STATE;
    Atom UTF8_STRING;
    constexpr static Atom _NET_WM_STATE_REMOVE = 0;
    constexpr static Atom _NET_WM_STATE_ADD = 1;
    constexpr static Atom _NET_WM_STATE_TOGGLE = 2;

    std::vector<Key> esdKeyTable;
    std::vector<unsigned int> x11KeyTable;

    Key fromX11KeyCode(unsigned int x11KeyCode);
    void initKeyTables();
};
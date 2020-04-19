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

#include <eseed/window/input.hpp>
#include <string>
#include <memory>
#include <functional>
#include <optional>

namespace esd::wnd {

struct WindowSize { int w, h; };
struct WindowPos { int x, y; };
struct MousePos { double x, y; };

struct KeyEvent { Key key; bool down; };
struct KeyCharEvent { char32_t codePoint; };
struct CursorMoveEvent { MousePos pos; MousePos screenPos; };
struct MouseButtonEvent { MouseButton button; bool down; };
struct ScrollEvent { double vScroll, hScroll; };

class Window {
public:
    std::function<void(KeyEvent)> keyHandler;
    std::function<void(KeyCharEvent)> keyCharHandler;
    std::function<void(CursorMoveEvent)> cursorMoveHandler;
    std::function<void(MouseButtonEvent)> mouseButtonHandler;
    std::function<void(ScrollEvent)> scrollHandler;

    Window(std::string title, WindowSize size, std::optional<WindowPos> pos = std::nullopt);
    ~Window();

    // Poll for window events
    void poll();

    // Wait for window events
    void waitEvents();

    std::string getTitle();
    void setTitle(std::string title);

    WindowSize getSize();
    void setSize(WindowSize size);

    WindowPos getPos();
    void setPos(WindowPos pos);

    bool isCloseRequested();
    void setCloseRequested(bool closeRequested);

    bool isFullscreen();
    void setFullscreen(bool fullscreen);

    bool isKeyDown(Key keyCode);

    // Get toggle state of applicable keys like caps lock or num lock
    // Always returns false for non-toggleable keys
    bool isKeyToggled(Key keyCode);

    MousePos getCursorPos();
    void setCursorPos(MousePos pos);

    MousePos getCursorScreenPos();
    void setCursorScreenPos(MousePos pos);

    bool isMouseButtonDown(MouseButton button);

protected:
    // Should be defined in the platform-specific source file with data members
    // and additional functions
    class Impl;
    std::unique_ptr<Impl> impl;
};

}
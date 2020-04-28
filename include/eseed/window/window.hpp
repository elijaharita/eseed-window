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
struct CursorPos { double x, y; };

struct KeyEvent { Key key; bool down; };
struct KeyCharEvent { char32_t codePoint; };
struct CursorMoveEvent { CursorPos pos; CursorPos screenPos; bool entered; };
struct CursorExitEvent {};
struct MouseButtonEvent { MouseButton button; bool down; };
struct ScrollEvent { double vScroll, hScroll; };
struct ResizeEvent { WindowSize size; };
struct MoveEvent { WindowPos pos; };

class Window {
public:
    Window(std::string title, WindowSize size, std::optional<WindowPos> pos = std::nullopt);
    Window(const Window&) = delete;
    ~Window();

    void setKeyHandler(std::function<void(KeyEvent)> handler) { keyHandler = handler; }
    void setKeyCharHandler(std::function<void(KeyCharEvent)> handler) { keyCharHandler = handler; }
    void setCursorMoveHandler(std::function<void(CursorMoveEvent)> handler) { cursorMoveHandler = handler; }
    void setCursorExitHandler(std::function<void(CursorExitEvent)> handler) { cursorExitHandler = handler; }
    void setMouseButtonHandler(std::function<void(MouseButtonEvent)> handler) { mouseButtonHandler = handler; }
    void setScrollHandler(std::function<void(ScrollEvent)> handler) { scrollHandler = handler; }
    void setResizeHandler(std::function<void(ResizeEvent)> handler) { resizeHandler = handler; }
    void setMoveHandler(std::function<void(MoveEvent)> handler) { moveHandler = handler; }

    // Close the window and release all resources
    // The window cannot be used again after this call
    void close();

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

    bool isKeyDown(Key key);

    // Get toggle state of applicable keys like caps lock or num lock
    // Always returns false for non-toggleable keys
    bool isKeyToggled(Key key);

    CursorPos getCursorPos();
    void setCursorPos(CursorPos pos);

    CursorPos getCursorScreenPos();
    void setCursorScreenPos(CursorPos pos);

    bool isMouseButtonDown(MouseButton button);

protected:
    // Should be defined in the platform-specific source file with data members
    // and additional functions
    class Impl;
    std::unique_ptr<Impl> impl;

    std::function<void(KeyEvent)> keyHandler;
    std::function<void(KeyCharEvent)> keyCharHandler;
    std::function<void(CursorMoveEvent)> cursorMoveHandler;
    std::function<void(CursorExitEvent)> cursorExitHandler;
    std::function<void(MouseButtonEvent)> mouseButtonHandler;
    std::function<void(ScrollEvent)> scrollHandler;
    std::function<void(ResizeEvent)> resizeHandler;
    std::function<void(MoveEvent)> moveHandler;
};

}
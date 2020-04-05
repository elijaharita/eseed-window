#pragma once

#include <eseed/window/keycode.hpp>
#include <string>
#include <memory>
#include <functional>
#include <optional>

namespace esd::window {

struct Size {
    int width, height;
};

struct Pos {
    int x, y;
};

struct KeyEvent {
    KeyCode keyCode;
    bool down;
};

class Window {
public:
    std::function<void(KeyEvent)> keyHandler;
    std::function<void(char)> keyCharHandler;

    Window(std::string title, Size size);
    ~Window();

    // Poll for window events
    void poll();

    // Get window size
    Size getSize();
    // Set window size
    void setSize(const Size& size);

    // Check whether the window close button has been pressed
    bool isCloseRequested();
    // Simulate window close button click
    void setCloseRequested(bool closeRequested);

private:
    // Should be defined in the platform-specific source file with data members
    // and additional functions
    class Impl;
    std::unique_ptr<Impl> impl;
};

}
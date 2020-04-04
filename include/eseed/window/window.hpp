#pragma once

#include <string>
#include <memory>

namespace esd::window {

struct Size {
    int width, height;
};

struct Pos {
    int x, y;
};

class Window {
public:
    Window(std::string title, Size size);
    ~Window();

    void poll();

    Size getSize();
    void setSize(Size size);

    bool isCloseRequested();

private:
    class Impl;
    std::unique_ptr<Impl> impl;
};

}
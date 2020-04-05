#include <eseed/window/window.hpp>

#include <chrono>
#include <thread>
#include <iostream>

int main() {
    esd::window::Window window("A Window", { 1366, 768 });
    window.keyHandler = [](esd::window::KeyEvent e) {
        if (e.down) std::cout << e.keyCode << std::endl;
    };

    float t = 0;

    // Check for window updates until the close button is pressed
    while (!window.isCloseRequested()) {
        window.poll();
    }
}
#include <eseed/window/window.hpp>

#include <chrono>
#include <thread>
#include <iostream>

using esd::window::KeyCode;

int main() {
    std::string title;
    
    esd::window::Window window("A Window", { 1366, 768 });
    
    window.keyHandler = [&](esd::window::KeyEvent e) {
        //std::cout << e.keyCode << ": " << e.down << std::endl;
        if (e.down) {
            if (e.keyCode == KeyCode::Escape)
                window.setCloseRequested(true);
            if (e.keyCode == KeyCode::Return)
                std::cout << std::endl;
        }
    };

    window.keyCharHandler = [&](char c) {
        std::string title = window.getTitle();
        if (c == 0x08) title = title.substr(0, title.length() - 1);
        else title += c;
        window.setTitle(title);
    };

    float t = 0;

    // Check for window updates until the close button is pressed
    while (!window.isCloseRequested()) {
        window.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
    }
}
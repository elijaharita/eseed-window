#include <eseed/window/window.hpp>

#include <chrono>
#include <thread>
#include <iostream>

using esd::wnd::KeyCode;

int main() {

    esd::wnd::Window window("A Window", { 1366, 768 });
    
    // Handles raw key presses and releases
    window.keyHandler = [&](esd::wnd::KeyEvent e) {
        
        // Only handle key down events, key up can be intercepted by checking
        // for !e.down (or just an else statement)
        if (e.down) {

            // Close window when escape is pressed
            // Many keys have alternate names
            // Escape is listed as both esd::window::KeyCode::Esc and
            // esd::window::KeyCode::Escape
            if (e.keyCode == KeyCode::Esc)
                window.setCloseRequested(true);
        }
    };

    // Handles interpreted character input 
    // (Shift + "1" causes "!" on US keyboard, etc.)
    window.keyCharHandler = [&](char c) {

        std::string title = window.getTitle();

        // If the ASCII code is BS (backspace), specially remove a character
        if (c == 0x08) title = title.substr(0, title.length() - 1);

        // Otherwise add the character to the string
        else title += c;

        window.setTitle(title);
    };

    float t = 0;

    // Check for window updates until the close button is pressed
    while (!window.isCloseRequested()) {
        window.poll();

        // Don't scronch cpu : )
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));
    }
}
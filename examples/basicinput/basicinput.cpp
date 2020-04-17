#include <eseed/window/window.hpp>

#include <chrono>
#include <thread>
#include <iostream>
#include <bitset>

using esd::wnd::KeyCode;

std::string codePointToUtf8(char32_t codePoint);

int main() {

    esd::wnd::Window window("A Window", { 1366, 768 });
    
    // Handles raw key presses and releases
    window.keyHandler = [&](esd::wnd::KeyEvent e) {
        
        // Only handle key down events, key up can be intercepted by checking
        // for !e.down (or just an else statement)
        if (e.down) {

            // Close window when escape is pressed
            // Many keys have alternate names
            // Escape is listed as both esd::wnd::KeyCode::Esc and
            // esd::wnd::KeyCode::Escape
            if (e.keyCode == KeyCode::Esc)
                window.setCloseRequested(true);
        }
    };

    // Handles interpreted character input 
    // (Shift + "1" causes "!" on US keyboard, etc.)
    window.keyCharHandler = [&](char32_t c) {

        std::string title = window.getTitle();

        std::cout << title << std::endl;

        // If the ASCII code is BS (backspace), specially remove a character
        if (c == 0x08) title = title.substr(0, title.length() - 1);

        // Otherwise add the character to the string
        else title += codePointToUtf8(c);

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

// thank you wikipedia, very cool
std::string codePointToUtf8(char32_t codePoint) {

    // 1 byte
    if (codePoint <= 0x7F) {
        std::string out(1, 0);
        out[0] = (char)(codePoint & 0x7F);
        return out;
    }

    // 2 bytes
    if (codePoint <= 0x07FF) {
        std::string out(2, 0);
        out[0] = (char)((codePoint >> 06 & 0x1F) + 0b11000000);
        out[1] = (char)((codePoint >> 00 & 0x3F) + 0b10000000);
        return out;
    }

    // 3 bytes
    if (codePoint <= 0xFFFF) {
        std::string out(3, 0);
        out[0] = (char)((codePoint >> 12 & 0x0F) + 0b11100000);
        out[1] = (char)((codePoint >> 06 & 0x3F) + 0b10000000);
        out[2] = (char)((codePoint >> 00 & 0x3F) + 0b10000000);
        return out;
    }

    // 4 bytes
    if (codePoint <= 0x10FFFF) {
        std::string out(4, 0);
        out[0] = (char)((codePoint >> 18 & 0x07) + 0b11110000);
        out[1] = (char)((codePoint >> 12 & 0x3F) + 0b10000000);
        out[2] = (char)((codePoint >> 06 & 0x3F) + 0b10000000);
        out[3] = (char)((codePoint >> 00 & 0x3F) + 0b10000000);
        return out;
    }

    throw std::runtime_error("Invalid code point");
}
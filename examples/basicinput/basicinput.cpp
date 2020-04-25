#include <eseed/window/window.hpp>

#include <chrono>
#include <thread>
#include <iostream>
#include <bitset>

using esd::wnd::Key;

std::string codePointToUtf8(char32_t codePoint);

int main() {

    esd::wnd::Window window("Start typing!", { 1366, 768 });

    std::string cursorPosString = "";
    std::string text = "Start typing!";
    
    // Handles raw key presses and releases
    window.setKeyHandler([&](esd::wnd::KeyEvent e) {
        
        // Only handle key down events, key up can be intercepted by checking 
        // for !e.down (or just an else statement)
        if (e.down) {

            // Close window when escape is pressed
            // Many keys have alternate names
            // Escape is listed as both esd::wnd::KeyCode::Esc and
            // esd::wnd::KeyCode::Escape
            if (e.key == Key::Esc)
                window.setCloseRequested(true);
        }
    });

    // Handles interpreted character input 
    // (Shift + "1" causes "!" on US keyboard, etc.)
    window.setKeyCharHandler([&](esd::wnd::KeyCharEvent e) {

        // If the code point is ASCII BS (backspace), specially remove a 
        // character
        if (e.codePoint == 0x08) text = text.substr(0, text.length() - 1);

        // Otherwise add the character to the string
        else text += codePointToUtf8(e.codePoint);

        window.setTitle(cursorPosString + " | " + text);
    });

    // Displays the cursor window coordinates in the beginning of the title
    window.setCursorMoveHandler([&](esd::wnd::CursorMoveEvent e) {
        cursorPosString = "Cursor Pos: " 
            + std::to_string(e.pos.x) 
            + " : " 
            + std::to_string(e.pos.y);

        window.setTitle(cursorPosString + " | " + text);
    });

    // Print mouse button press and release events to the console
    window.setMouseButtonHandler([&](esd::wnd::MouseButtonEvent e) {
        std::cout 
            << "Mouse button " 
            << static_cast<int>(e.button)
            << " "
            << (e.down ? "pressed" : "released")
            << std::endl;
    });

    // Move window based on scroll
    window.setScrollHandler([&](esd::wnd::ScrollEvent e) {
        auto pos = window.getPos();
        pos.x += static_cast<int>(e.hScroll * 30);
        pos.y -= static_cast<int>(e.vScroll * 30);
        window.setPos(pos);
    });

    // Check for window updates until the close button is pressed
    while (!window.isCloseRequested()) {
        window.waitEvents();
    }
}

// thank you wikipedia, very cool
std::string codePointToUtf8(char32_t codePoint) {

    // 1 byte
    if (codePoint <= 0x7F) {
        std::string out(1, 0);
        out[0] = static_cast<int>(codePoint & 0x7F);
        return out;
    }

    // 2 bytes
    if (codePoint <= 0x07FF) {
        std::string out(2, 0);
        out[0] = static_cast<int>((codePoint >> 06 & 0x1F) + 0b11000000);
        out[1] = static_cast<int>((codePoint >> 00 & 0x3F) + 0b10000000);
        return out;
    }

    // 3 bytes
    if (codePoint <= 0xFFFF) {
        std::string out(3, 0);
        out[0] = static_cast<int>((codePoint >> 12 & 0x0F) + 0b11100000);
        out[1] = static_cast<int>((codePoint >> 06 & 0x3F) + 0b10000000);
        out[2] = static_cast<int>((codePoint >> 00 & 0x3F) + 0b10000000);
        return out;
    }

    // 4 bytes
    if (codePoint <= 0x10FFFF) {
        std::string out(4, 0);
        out[0] = static_cast<int>((codePoint >> 18 & 0x07) + 0b11110000);
        out[1] = static_cast<int>((codePoint >> 12 & 0x3F) + 0b10000000);
        out[2] = static_cast<int>((codePoint >> 06 & 0x3F) + 0b10000000);
        out[3] = static_cast<int>((codePoint >> 00 & 0x3F) + 0b10000000);
        return out;
    }

    throw std::runtime_error("Invalid code point");
}
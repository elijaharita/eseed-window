#include <eseed/window/window.hpp>
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

#include <iostream>
#include <thread>

int main() {
    esd::wnd::Window window("Dimensions!", { 1366, 768 });

    window.keyHandler = [&](esd::wnd::KeyEvent e) {
        if (e.down) {
            // F11: Toggle fullscreen
            if (e.keyCode == esd::wnd::KeyCode::F11) {
                window.setFullscreen(!window.isFullscreen());
            } 
            
            // Right: Move window to the right
            else if (e.keyCode == esd::wnd::KeyCode::Right) {
                auto pos = window.getPos();
                window.setPos({ pos.x + 50, pos.y });
            } 
            
            // Left: Move window to the left
            else if (e.keyCode == esd::wnd::KeyCode::Left) {
                auto pos = window.getPos();
                window.setPos({ pos.x - 50, pos.y });
            } 
            
            // Down: Move window down
            else if (e.keyCode == esd::wnd::KeyCode::Down) {
                auto pos = window.getPos();
                window.setPos({ pos.x, pos.y + 50 });
            } 
            
            // Up: Move window up
            else if (e.keyCode == esd::wnd::KeyCode::Up) {
                auto pos = window.getPos();
                window.setPos({ pos.x, pos.y - 50 });
            } 
            
            // Space: Increase size
            else if (e.keyCode == esd::wnd::KeyCode::Space) {
                auto size = window.getSize();
                window.setSize({ size.w + 50, size.h + 50 });
            } 
            
            // Left Shift: Decrease size
            else if (e.keyCode == esd::wnd::KeyCode::LShift) {
                auto size = window.getSize();
                window.setSize({ size.w - 50, size.h - 50 });
            }

            // Escape: Close
            else if (e.keyCode == esd::wnd::KeyCode::Escape) {
                window.setCloseRequested(true);
            }
        }
    };

    while (!window.isCloseRequested()) {
        // Sleep for a sixtieth of a second so cpu doesn't evaporate :)
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / 60));

        window.poll();
    }
}
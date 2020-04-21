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

int main() {
    esd::wnd::Window window("Dimensions!", { 1366, 768 });
    std::cout << "title: " << window.getTitle() << std::endl;

    window.keyHandler = [&](esd::wnd::KeyEvent e) {
        if (e.down) {
            // F11: Toggle fullscreen
            if (e.key == esd::wnd::Key::F11) {
                window.setFullscreen(!window.isFullscreen());
            } 
            
            // Right: Move window to the right
            else if (e.key == esd::wnd::Key::Right) {
                auto pos = window.getPos();
                window.setPos({ pos.x + 50, pos.y });
            } 
            
            // Left: Move window to the left
            else if (e.key == esd::wnd::Key::Left) {
                auto pos = window.getPos();
                window.setPos({ pos.x - 50, pos.y });
            } 
            
            // Down: Move window down
            else if (e.key == esd::wnd::Key::Down) {
                auto pos = window.getPos();
                window.setPos({ pos.x, pos.y + 50 });
            } 
            
            // Up: Move window up
            else if (e.key == esd::wnd::Key::Up) {
                auto pos = window.getPos();
                window.setPos({ pos.x, pos.y - 50 });
            } 
            
            // Space: Increase size
            else if (e.key == esd::wnd::Key::Space) {
                auto size = window.getSize();
                window.setSize({ size.w + 50, size.h + 50 });
            } 
            
            // Left Shift: Decrease size
            else if (e.key == esd::wnd::Key::LShift) {
                auto size = window.getSize();
                window.setSize({ size.w - 50, size.h - 50 });
            }

            // Escape: Close
            else if (e.key == esd::wnd::Key::Escape) {
                window.setCloseRequested(true);
            }
        }
    };

    while (!window.isCloseRequested()) {
        window.waitEvents();
    }
}
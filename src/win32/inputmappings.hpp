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
#include <windows.h>
#include <map>

namespace esd::wnd {

const std::map<UINT, MouseButton> mouseButtonMappings = {
    { VK_LBUTTON, MouseButton::LButton },
    { VK_RBUTTON, MouseButton::RButton },
    { VK_MBUTTON, MouseButton::MButton },
    { VK_XBUTTON1, MouseButton::XButton1 },
    { VK_XBUTTON2, MouseButton::XButton2 }
};

const std::map<UINT, Key> keyMappings = {
    { 0x30, Key::Num0 },
    { 0x31, Key::Num1 },
    { 0x32, Key::Num2 },
    { 0x33, Key::Num3 },
    { 0x34, Key::Num4 },
    { 0x35, Key::Num5 },
    { 0x36, Key::Num6 },
    { 0x37, Key::Num7 },
    { 0x38, Key::Num8 },
    { 0x39, Key::Num9 },
    { 0x41, Key::A },
    { 0x42, Key::B },
    { 0x43, Key::C },
    { 0x44, Key::D },
    { 0x45, Key::E },
    { 0x46, Key::F },
    { 0x47, Key::G },
    { 0x48, Key::H },
    { 0x49, Key::I },
    { 0x4a, Key::J },
    { 0x4b, Key::K },
    { 0x4c, Key::L },
    { 0x4d, Key::M },
    { 0x4e, Key::N },
    { 0x4f, Key::O },
    { 0x50, Key::P },
    { 0x51, Key::Q },
    { 0x52, Key::R },
    { 0x53, Key::S },
    { 0x54, Key::T },
    { 0x55, Key::U },
    { 0x56, Key::V },
    { 0x57, Key::W },
    { 0x58, Key::X },
    { 0x59, Key::Y },
    { 0x5a, Key::Z },
    { VK_BACK, Key::Backspace },
    { VK_TAB, Key::Tab },
    { VK_CLEAR, Key::Clear },
    { VK_RETURN, Key::Return },
    { VK_LSHIFT, Key::LShift },
    { VK_RSHIFT, Key::RShift },
    { VK_LCONTROL, Key::LControl },
    { VK_RCONTROL, Key::RControl },
    { VK_LMENU, Key::LAlt },
    { VK_RMENU, Key::RAlt },
    { VK_PAUSE, Key::Pause },
    { VK_CAPITAL, Key::CapsLock },
    { VK_ESCAPE, Key::Esc },
    { VK_SPACE, Key::Space },
    { VK_PRIOR, Key::PageUp },
    { VK_NEXT, Key::PageDown },
    { VK_END, Key::End },
    { VK_HOME, Key::Home },
    { VK_LEFT, Key::Left },
    { VK_UP, Key::Up },
    { VK_RIGHT, Key::Right },
    { VK_DOWN, Key::Down },
    { VK_SELECT, Key::Select },
    { VK_SNAPSHOT, Key::PrintScreen },
    { VK_INSERT, Key::Insert },
    { VK_DELETE, Key::Delete },
    { VK_APPS, Key::Menu },
    { VK_LWIN, Key::LMeta },
    { VK_RWIN, Key::RMeta },
    { VK_NUMPAD0, Key::Numpad0 },
    { VK_NUMPAD1, Key::Numpad1 },
    { VK_NUMPAD2, Key::Numpad2 },
    { VK_NUMPAD3, Key::Numpad3 },
    { VK_NUMPAD4, Key::Numpad4 },
    { VK_NUMPAD5, Key::Numpad5 },
    { VK_NUMPAD6, Key::Numpad6 },
    { VK_NUMPAD7, Key::Numpad7 },
    { VK_NUMPAD8, Key::Numpad8 },
    { VK_NUMPAD9, Key::Numpad9 },
    { VK_MULTIPLY, Key::Multiply },
    { VK_ADD, Key::Add },
    { VK_SUBTRACT, Key::Subtract },
    { VK_DECIMAL, Key::Decimal },
    { VK_DIVIDE, Key::Divide },
    { VK_F1, Key::F1 },
    { VK_F2, Key::F2 },
    { VK_F3, Key::F3 },
    { VK_F4, Key::F4 },
    { VK_F5, Key::F5 },
    { VK_F6, Key::F6 },
    { VK_F7, Key::F7 },
    { VK_F8, Key::F8 },
    { VK_F9, Key::F9 },
    { VK_F10, Key::F10 },
    { VK_F11, Key::F11 },
    { VK_F12, Key::F12 },
    { VK_F13, Key::F13 },
    { VK_F14, Key::F14 },
    { VK_F15, Key::F15 },
    { VK_F16, Key::F16 },
    { VK_F17, Key::F17 },
    { VK_F18, Key::F18 },
    { VK_F19, Key::F19 },
    { VK_F20, Key::F20 },
    { VK_F21, Key::F21 },
    { VK_F22, Key::F22 },
    { VK_F23, Key::F23 },
    { VK_F24, Key::F24 },
    { VK_NUMLOCK, Key::NumLock },
    { VK_SCROLL, Key::ScrollLock },
    { VK_OEM_1, Key::Semicolon },
    { VK_OEM_PLUS, Key::Equal },
    { VK_OEM_COMMA, Key::Comma },
    { VK_OEM_MINUS, Key::Dash },
    { VK_OEM_PERIOD, Key::Period },
    { VK_OEM_2, Key::Slash },
    { VK_OEM_3, Key::Backtick },
    { VK_OEM_4, Key::LBracket },
    { VK_OEM_5, Key::Backslash },
    { VK_OEM_6, Key::RBracket },
    { VK_OEM_7, Key::Apostrophe },
};

}
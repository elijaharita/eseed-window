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
#include <map>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>

namespace esd::wnd {

const std::map<std::string, Key> keyMappings = {
    { "AE01", Key::Num1 },
    { "AE02", Key::Num2 },
    { "AE03", Key::Num3 },
    { "AE04", Key::Num4 },
    { "AE05", Key::Num5 },
    { "AE06", Key::Num6 },
    { "AE07", Key::Num7 },
    { "AE08", Key::Num8 },
    { "AE09", Key::Num9 },
    { "AE10", Key::Num0 },
    { "AD01", Key::Q },
    { "AD02", Key::W },
    { "AD03", Key::E },
    { "AD04", Key::R },
    { "AD05", Key::T },
    { "AD06", Key::Y },
    { "AD07", Key::U },
    { "AD08", Key::I },
    { "AD09", Key::O },
    { "AD10", Key::P },
    { "AD11", Key::LBracket },
    { "AD12", Key::RBracket },
    { "AC01", Key::A },
    { "AC02", Key::S },
    { "AC03", Key::D },
    { "AC04", Key::F },
    { "AC05", Key::G },
    { "AC06", Key::H },
    { "AC07", Key::J },
    { "AC08", Key::K },
    { "AC09", Key::L },
    { "AC10", Key::Semicolon },
    { "AC11", Key::Apostrophe },
    { "AB01", Key::Z },
    { "AB02", Key::X },
    { "AB03", Key::C },
    { "AB04", Key::V },
    { "AB05", Key::B },
    { "AB06", Key::N },
    { "AB07", Key::M },
    { "AB08", Key::Comma },
    { "AB09", Key::Period },
    { "AB10", Key::Slash },
    { "ESC", Key::Escape },
    { "FK01", Key::F1 },
    { "FK02", Key::F2 },
    { "FK03", Key::F3 },
    { "FK04", Key::F4 },
    { "FK05", Key::F5 },
    { "FK06", Key::F6 },
    { "FK07", Key::F7 },
    { "FK08", Key::F8 },
    { "FK09", Key::F9 },
    { "FK10", Key::F10 },
    { "FK11", Key::F11 },
    { "FK12", Key::F12 },
    { "PRSC", Key::PrintScreen },
    { "SCLK", Key::ScrollLock },
    { "PAUS", Key::Pause},
    { "TLDE", Key::Backtick },
    { "BKSP", Key::Backspace },
    { "TAB", Key::Tab },
    { "BKSL", Key::Backslash },
    { "CAPS", Key::CapsLock },
    { "RTRN", Key::Return },
    { "LFSH", Key::LShift },
    { "RTSH", Key::RShift },
    { "LCTL", Key::LControl },
    { "LWIN", Key::LMeta },
    { "LALT", Key::LAlt },
    { "SPCE", Key::Space },
    { "RLAT", Key::RAlt },
    { "RWIN", Key::RMeta },
    { "MENU", Key::Context },
    { "RCTL", Key::RCtrl },
    { "INS", Key::Insert },
    { "HOME", Key::Home },
    { "PGUP", Key::PageUp },
    { "DELE", Key::Delete },
    { "END", Key::End },
    { "PGDN", Key::PageDown },
    { "UP", Key::Up },
    { "LEFT", Key::Left },
    { "DOWN", Key::Down },
    { "RGHT", Key::Right },
    { "NMLK", Key::NumLock },
    { "KPDV", Key::Divide },
    { "KPMU", Key::Multiply },
    { "KPSU", Key::Subtract },
    { "KPAD", Key::Add },
    { "KPEN", Key::Enter },
    { "KPDL", Key::Decimal },
    { "KP0", Key::Numpad0 },
    { "KP1", Key::Numpad1 },
    { "KP2", Key::Numpad2 },
    { "KP3", Key::Numpad3 },
    { "KP4", Key::Numpad4 },
    { "KP5", Key::Numpad5 },
    { "KP6", Key::Numpad6 },
    { "KP7", Key::Numpad7 },
    { "KP8", Key::Numpad8 },
    { "KP9", Key::Numpad9 },
};

}
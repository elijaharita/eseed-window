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

#include <string>

namespace esd::wnd {

enum struct MouseButton {
    LButton = 1,
    RButton = 2,
    MButton = 3,
    XButton1 = 4,
    XButton2 = 5
};

enum struct Key {
    Unknown = 0,
    Backspace = 8,
        Back = Backspace,
    Tab = 9,
    Return = 10,
        Enter = Return,
    Escape = 27,
        Esc = Escape,
    Space = 32,
    Apostrophe = 39, // ' "
        Quote = Apostrophe, // ' "
    Comma = 44, // , <
        Less = Comma, // , <
    Dash = 45, // - _
        Minus = Dash, // - _
        Underscore = Dash, // - _
    Period = 46, // . >
        Greater = Period, // . >
    Slash = 47, // / ?
        QuestionMark = Slash, // / ?
    Num0 = 48,
        RParenthesis = Num0,
        RParen = RParenthesis,
    Num1 = 49,
        ExclamationMark = Num1,
    Num2 = 50,
        AtSign = Num2,
    Num3 = 51,
        Hash = Num3,
    Num4 = 52,
        Dollar = Num4,
    Num5 = 53,
        Percent = Num5,
    Num6 = 54,
        Caret = Num6,
    Num7 = 55,
        Ampersand = Num7,
        Amp = Ampersand,
    Num8 = 56,
        Asterisk = Num8,
    Num9 = 57,
        LParenthesis = Num9,
        LParen = LParenthesis,
    Semicolon = 59, // : ;
        Colon = Semicolon, // : ;
    Equal = 61, // = +
        Plus = Equal, // = +
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LBracket = 91, // [ {
        LBrace = LBracket,
    Backslash = 92, // \ |
        Pipe = Backslash,
    RBracket = 93, // ] }
        RBrace = RBracket,
    Backtick = 96, // ` ~
        Grave = Backtick,
        Tilde = Backtick,
    Delete = 127,
        Del = Delete,
    Clear,
    LShift,
    RShift,
    LControl,
        LCtrl = LControl,
    RControl,
        RCtrl = RControl,
    LAlt,
    RAlt,
    Pause,
    CapsLock,
        CapsLk = CapsLock,
        Caps = CapsLock,
    PageUp,
        PgUp = PageUp,
    PageDown,
        PgDn = PageDown,
    End,
    Home,
    Left, // Left Arrow
    Up, // Up Arrow
    Right, // Right Arrow
    Down, // Down Arrow
    Select,
    PrintScreen,
        PrtScr = PrintScreen,
    Insert,
        Ins = Insert,
    LMeta,
    RMeta,
    Menu,
        Context = Menu,
    Numpad0,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9,
    Multiply, // Numpad *
        Mul = Multiply, // Numpad *
    Add, // Numpad +
    Subtract, // Numpad -
        Sub = Subtract, // Numpad -
    Decimal, // Numpad .
        Dec = Decimal, // Numpad .
    Divide, // Numpad /
        Div = Divide, // Numpad /
    F1,
    F2, 
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    NumLock,
        NumLk = NumLock,
    ScrollLock,
        ScrLk = ScrollLock,
    LastKey = ScrollLock
};

inline std::string getKeyName(Key key) {
    switch (key) {
    case Key::Backspace: return "Backspace";
    case Key::Tab: return "Tab";
    case Key::Return: return "Return";
    case Key::Escape: return "Escape";
    case Key::Space: return "Space";
    case Key::Apostrophe: return "Apostrophe";
    case Key::Comma: return "Comma";
    case Key::Dash: return "Dash";
    case Key::Period: return "Period";
    case Key::Slash: return "Slash";
    case Key::Num0: return "0";
    case Key::Num1: return "1";
    case Key::Num2: return "2";
    case Key::Num3: return "3";
    case Key::Num4: return "4";
    case Key::Num5: return "5";
    case Key::Num6: return "6";
    case Key::Num7: return "7";
    case Key::Num8: return "8";
    case Key::Num9: return "9";
    case Key::Semicolon: return "Semicolon";
    case Key::Equal: return "Equal";
    case Key::A: return "A";
    case Key::B: return "B";
    case Key::C: return "C";
    case Key::D: return "D";
    case Key::E: return "E";
    case Key::F: return "F";
    case Key::G: return "G";
    case Key::H: return "H";
    case Key::I: return "I";
    case Key::J: return "J";
    case Key::K: return "K";
    case Key::L: return "L";
    case Key::M: return "M";
    case Key::N: return "N";
    case Key::O: return "O";
    case Key::P: return "P";
    case Key::Q: return "Q";
    case Key::R: return "R";
    case Key::S: return "S";
    case Key::T: return "T";
    case Key::U: return "U";
    case Key::V: return "V";
    case Key::W: return "W";
    case Key::X: return "X";
    case Key::Y: return "Y";
    case Key::Z: return "Z";
    case Key::LBracket: return "Left Bracket";
    case Key::Backslash: return "Backslash";
    case Key::RBracket: return "Right Bracket";
    case Key::Backtick: return "Backtick";
    case Key::Delete: return "Delete";
    case Key::Clear: return "Clear";
    case Key::LShift: return "Left Shift";
    case Key::RShift: return "Right Shift";
    case Key::LControl: return "Left Control";
    case Key::RControl: return "Right Control";
    case Key::LAlt: return "Left Alt";
    case Key::RAlt: return "Right Alt";
    case Key::Pause: return "Pause";
    case Key::CapsLock: return "Caps Lock";
    case Key::PageUp: return "Page Up";
    case Key::PageDown: return "Page Down";
    case Key::End: return "End";
    case Key::Home: return "Home";
    case Key::Left: return "Left";
    case Key::Up: return "Up";
    case Key::Right: return "Right";
    case Key::Down: return "Down";
    case Key::Select: return "Select";
    case Key::PrintScreen: return "Print Screen";
    case Key::Insert: return "Insert";
    case Key::LMeta: return "Left Meta";
    case Key::RMeta: return "Right Meta";
    case Key::Menu: return "Menu";
    case Key::Numpad0: return "Numpad 0";
    case Key::Numpad1: return "Numpad 1";
    case Key::Numpad2: return "Numpad 2";
    case Key::Numpad3: return "Numpad 3";
    case Key::Numpad4: return "Numpad 4";
    case Key::Numpad5: return "Numpad 5";
    case Key::Numpad6: return "Numpad 6";
    case Key::Numpad7: return "Numpad 7";
    case Key::Numpad8: return "Numpad 8";
    case Key::Numpad9: return "Numpad 9";
    case Key::Multiply: return "Multiply";
    case Key::Add: return "Add";
    case Key::Subtract: return "Subtract";
    case Key::Decimal: return "Decimal";
    case Key::Divide: return "Divide";
    case Key::F1: return "F1";
    case Key::F2: return "F2";
    case Key::F3: return "F3";
    case Key::F4: return "F4";
    case Key::F5: return "F5";
    case Key::F6: return "F6";
    case Key::F7: return "F7";
    case Key::F8: return "F8";
    case Key::F9: return "F9";
    case Key::F10: return "F10";
    case Key::F11: return "F11";
    case Key::F12: return "F12";
    case Key::F13: return "F13";
    case Key::F14: return "F14";
    case Key::F15: return "F15";
    case Key::F16: return "F16";
    case Key::F17: return "F17";
    case Key::F18: return "F18";
    case Key::F19: return "F19";
    case Key::F20: return "F20";
    case Key::F21: return "F21";
    case Key::F22: return "F22";
    case Key::F23: return "F23";
    case Key::F24: return "F24";
    case Key::NumLock: return "Num Lock";
    case Key::ScrollLock: return "Scroll Lock";
    }

    return "Unknown Key";
}

}
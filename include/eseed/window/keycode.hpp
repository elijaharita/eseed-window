#pragma once

namespace esd::window {

enum KeyCode {
    Unknown = 0,
    Backspace = 8,
        Back = Backspace,
    Tab = 9,
    Escape = 27,
        Esc = Escape,
    Space = 32,
    Quote = 39, // ' "
    Comma = 44, // , <
        Less = Comma, // , <
    Dash = 45, // - _
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
        LBrace = LBracket, // [ {
    Backslash = 92, // \ |
        Pipe = Backslash, // \ |
    RBracket = 93, // ] }
        RBrace = RBracket, // ] }
    Backtick = 96, // ` ~
        Grave = Backtick, // ` ~
        Tilde = Backtick, // ` ~
    Delete = 127,
        Del = Delete,
    Clear,
    Return,
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
};

}
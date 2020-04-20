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

#include <eseed/window/window.hpp>
#include <windows.h>
#include <winuser.h>

class esd::wnd::Window::Impl {
public:
    HINSTANCE hInstance;
    HWND hWnd;
    WINDOWPLACEMENT windowedPlacement;
    bool closeRequested;
    
    // Convert window client dimensions to Win32 window RECT
    RECT createWindowRect(WindowSize size, WindowPos pos = {});

    // Get VKey from a Win32 RAWKEYBOARD with differentiated left and right 
    // modifier keys 
    // (e.g. VK_LSHIFT and VK_RSHIFT instead of VK_SHIFT)
    static UINT extractDiffWin32KeyCode(const RAWKEYBOARD& rawKeyboard);

    // Convert a Win32 virtual key code to esd::wnd key code
    static Key fromWin32KeyCode(UINT win32KeyCode);

    // Convert an esd::wnd key code to Win32 virtual key code
    static UINT toWin32KeyCode(Key keyCode);

    // Win32 WNDPROC
    static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Win32 HOOKPROC
    static LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

    static std::string wideStringToString(const std::wstring& wstring);
    static std::wstring stringToWideString(const std::string& string);
};
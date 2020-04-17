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

#include <eseed/window/window.hpp>

#include "keycodemappings.hpp"
#include "impl.hpp"
#include <Windows.h>
#include <iostream>
#include <set>

using namespace esd::wnd;

Window::Window(std::string title, Size size) {

    impl = std::make_unique<Impl>();

    impl->hInstance = GetModuleHandleW(NULL);
    
    constexpr wchar_t className[] = L"ESeed Window";
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = impl->hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = className;
    wc.lpfnWndProc = (WNDPROC)Impl::wndProc;
    wc.cbWndExtra = sizeof(Window*);
    RegisterClassExW(&wc);

    RECT rect = Impl::createWindowRect(size);
    impl->hWnd = CreateWindowExW(
        NULL,
        className,
        Impl::stringToWideString(title).c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL,
        NULL,
        impl->hInstance,
        NULL
    );

    // Window is null if failed to initialize
    if (impl->hWnd == NULL)
        throw std::runtime_error("Failed to create native Win32 window");

    // Set "this" pointer in window user data for use in WNDPROC
    SetWindowLongPtrW(impl->hWnd, GWLP_USERDATA, (LONG_PTR)this);

    ShowWindow(impl->hWnd, SW_SHOW);

    // Register raw input devices

    // Usage Reference:
    // https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

    RAWINPUTDEVICE rid[] = {
        { 0x01, 0x06, NULL, impl->hWnd }, // Keyboard
        { 0x01, 0x07, NULL, impl->hWnd }, // Keypad (Numpad)
    };

    RegisterRawInputDevices(rid, sizeof(rid) / sizeof(rid[0]), sizeof(rid[0]));
}

Window::~Window() {
    if (impl->hWnd) DestroyWindow(impl->hWnd);
}

void Window::poll() {
    MSG msg;
    while (PeekMessageW(&msg, impl->hWnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
}

std::string Window::getTitle() {

    int length = GetWindowTextLengthW(impl->hWnd);

    std::wstring wtitle = std::wstring(length, 0);
    GetWindowTextW(impl->hWnd, (LPWSTR)wtitle.c_str(), length + 1);

    return Impl::wideStringToString(wtitle);
}

void Window::setTitle(std::string title) {
    SetWindowTextW(impl->hWnd, Impl::stringToWideString(title).c_str());
}

Size Window::getSize() {
    RECT rect; 
    GetClientRect(impl->hWnd, &rect);
    return Size { rect.right - rect.left, rect.bottom - rect.top };
}

// TODO: retain window position
void Window::setSize(const Size& size) {
    RECT rect = Impl::createWindowRect(size);
    SetWindowPos(
        impl->hWnd, HWND_TOP,
        0,
        0,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL
    );
}

bool Window::isCloseRequested() {
    return impl->closeRequested;
}

void Window::setCloseRequested(bool closeRequested) {
    impl->closeRequested = closeRequested;
}

RECT Window::Impl::createWindowRect(Size size) {
    RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = size.width;
    rect.bottom = size.height;
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW ^ WS_OVERLAPPED, FALSE, NULL);
    return rect;
}

// TODO: improve key mapping performance
// Probably use two arrays instead of a map, barely any more memory, faster
// mapping
UINT Window::Impl::toWin32KeyCode(KeyCode keyCode) {
    for (const auto& it : keyCodeMappings)
        if (it.second == keyCode) return it.first;
    return NULL;
}

UINT Window::Impl::extractDiffWin32KeyCode(const RAWKEYBOARD& rawKeyboard) {
    // Differentiate left and right modifier keys
    bool extended = rawKeyboard.Flags & RI_KEY_E0;
    switch (rawKeyboard.VKey) {
    case VK_SHIFT:
        return MapVirtualKeyW(rawKeyboard.MakeCode, MAPVK_VSC_TO_VK_EX);
    case VK_CONTROL:
        return extended ? VK_RCONTROL : VK_LCONTROL;
    case VK_MENU:
        return extended ? VK_RMENU : VK_LMENU;
    default:
        return rawKeyboard.VKey;
    }
}

// TODO: improve key mapping performance
KeyCode Window::Impl::fromWin32KeyCode(UINT win32KeyCode) {
    auto it = keyCodeMappings.find((int)win32KeyCode);
    return it == keyCodeMappings.end() ? KeyCode::Unknown : it->second;
}

LRESULT CALLBACK Window::Impl::wndProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
) {
    auto window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (uMsg) {
    case WM_CLOSE:
        // Signal close request
        window->impl->closeRequested = true;

        // Override default close behavior
        return 0;
    case WM_CHAR:
        {
            if (window->keyCharHandler) window->keyCharHandler((char32_t)wParam);
        }
        return 0;
    case WM_INPUT:
        {
            RAWINPUT rawInput;
            UINT rawInputSize = sizeof(rawInput);

            GetRawInputData(
                (HRAWINPUT)lParam, 
                RID_INPUT, 
                &rawInput, 
                &rawInputSize, 
                sizeof(RAWINPUTHEADER)
            );

            // Key pressed / released
            if (rawInput.header.dwType == RIM_TYPEKEYBOARD) {

                // Skip unmapped keys
                if (rawInput.data.keyboard.VKey == 0xFF) break;
                
                KeyEvent event;
                event.down = !(rawInput.data.keyboard.Flags & RI_KEY_BREAK);
                event.keyCode = fromWin32KeyCode(
                    extractDiffWin32KeyCode(rawInput.data.keyboard)
                );
                if (window->keyHandler) window->keyHandler(event);
            }
        }
        break;
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK Window::Impl::lowLevelKeyboardProc(
    int nCode, WPARAM wParam, LPARAM lParam
) {
    return nCode;
}

std::string Window::Impl::wideStringToString(const std::wstring& wstring) {
    int length = WideCharToMultiByte(
        CP_UTF8, 
        NULL, 
        wstring.c_str(), 
        -1, 
        NULL, 
        NULL, 
        NULL, 
        NULL
    );

    std::string string(length - 1, 0);

    WideCharToMultiByte(
        CP_UTF8,
        NULL,
        wstring.c_str(),
        -1,
        (LPSTR)string.c_str(),
        length,
        NULL,
        NULL
    );

    return string;
}

std::wstring Window::Impl::stringToWideString(const std::string& string) {
    int wlength = MultiByteToWideChar(
        CP_UTF8,
        NULL,
        string.c_str(),
        -1,
        NULL,
        NULL
    );

    std::wstring wstring(wlength - 1, 0);

    MultiByteToWideChar(
        CP_UTF8,
        NULL,
        string.c_str(),
        -1,
        (LPWSTR)wstring.c_str(),
        wlength
    );

    return wstring;
}
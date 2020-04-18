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

#include "inputmappings.hpp"
#include "impl.hpp"
#include <Windows.h>
#include <windowsx.h>
#include <iostream>
#include <set>

using namespace esd::wnd;

Window::Window(std::string title, Size size, std::optional<Pos> pos) {

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
    wc.cbWndExtra = sizeof(this);
    RegisterClassExW(&wc);

    RECT rect = impl->createWindowRect(size, *pos);
    impl->hWnd = CreateWindowExW(
        NULL,
        className,
        Impl::stringToWideString(title).c_str(),
        WS_OVERLAPPEDWINDOW,
        // Use automatic Win32 window placement if no position provided
        pos ? rect.left : CW_USEDEFAULT,
        pos ? rect.top : CW_USEDEFAULT,
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

void Window::waitEvents() {
    MSG msg;

    // Wait for one message
    GetMessageW(&msg, impl->hWnd, 0, 0);
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
    
    // Read any remaining buffered messages
    poll();
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

void Window::setSize(Size size) {

    if (isFullscreen()) return;
    
    RECT rect = impl->createWindowRect(size, getPos());

    int diffX = size.w - rect.right - rect.left;
    int diffY = size.h - rect.bottom - rect.top;

    SetWindowPos(
        impl->hWnd, 
        NULL,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        NULL
    );
}

Pos Window::getPos() {
    RECT rect = { 0, 0 };
    ClientToScreen(impl->hWnd, (POINT*)&rect);
    return { rect.left, rect.top };
}

void Window::setPos(Pos pos) {
    
    if (isFullscreen()) return;

    RECT rect = impl->createWindowRect(getSize(), pos);
    
    SetWindowPos(
        impl->hWnd, 
        NULL,
        rect.left,
        rect.top,
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

bool Window::isFullscreen() {
    // Fullscreen won't have overlapped window style
    return !(GetWindowLong(impl->hWnd, GWL_STYLE) & WS_OVERLAPPEDWINDOW);
}

void Window::setFullscreen(bool fullscreen) {

    if (fullscreen) {
        MONITORINFO mi = { sizeof(mi) };
        GetMonitorInfo(
            MonitorFromWindow(impl->hWnd, MONITOR_DEFAULTTOPRIMARY), &mi
        );

        // Remove overlapped window style
        DWORD style = GetWindowLong(impl->hWnd, GWL_STYLE);
        SetWindowLong(impl->hWnd, GWL_STYLE, style & ~WS_OVERLAPPEDWINDOW);

        GetWindowPlacement(impl->hWnd, &impl->windowedPlacement);

        // Make window cover full monitor screen
        SetWindowPos(
            impl->hWnd, 
            HWND_TOP, 
            mi.rcMonitor.left,
            mi.rcMonitor.top,
            mi.rcMonitor.right - mi.rcMonitor.left,
            mi.rcMonitor.bottom - mi.rcMonitor.top,
            SWP_NOOWNERZORDER | SWP_FRAMECHANGED
        );
    } else {

        // Add back overlapped window style
        DWORD style = GetWindowLong(impl->hWnd, GWL_STYLE);
        SetWindowLong(impl->hWnd, GWL_STYLE, style | WS_OVERLAPPEDWINDOW);
        
        SetWindowPlacement(impl->hWnd, &impl->windowedPlacement);
        
        // Reapply stored dimensions
        SetWindowPos(
            impl->hWnd,
            NULL,
            0, 0, 0, 0,
            SWP_NOMOVE
                | SWP_NOSIZE
                | SWP_NOZORDER
                | SWP_NOOWNERZORDER
                | SWP_FRAMECHANGED
        );
    }

    InvalidateRect(impl->hWnd, NULL, TRUE);
}

bool Window::isKeyDown(Key keyCode) {
    for (auto it : keyMappings) {
        if (it.second == keyCode) return GetKeyState(it.first) & 0x8000;
    }
    throw std::runtime_error("Unknown key");
}

bool Window::isKeyToggled(Key key) {
    if (key != Key::CapsLock || key != Key::NumLock || key != Key::ScrollLock) 
        return false;
        
    for (auto it : keyMappings) {
        if (it.second == key) return GetKeyState(it.first) & 0x0001;
    }
}

Pos Window::getCursorPos() {
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(impl->hWnd, &point);
    return Pos { point.x, point.y };
}

void Window::setCursorPos(Pos pos) {
    POINT point = { pos.x, pos.y };
    ClientToScreen(impl->hWnd, &point);
    SetCursorPos(point.x, point.y);
}

Pos Window::getCursorScreenPos() {
    POINT point;
    GetCursorPos(&point);
    return Pos { point.x, point.y };
}

void Window::setCursorScreenPos(Pos pos) {
    SetCursorPos(pos.x, pos.y);
}

bool Window::isMouseButtonDown(MouseButton button) {
    for (auto it : mouseButtonMappings) {
        if (it.second == button) return GetKeyState(it.first) & 0x8000;
    }
    throw std::runtime_error("Unknown mouse button");
}

RECT Window::Impl::createWindowRect(Size size, Pos pos) {
    RECT rect;
    rect.left = pos.x;
    rect.top = pos.y;
    rect.right = pos.x + size.w;
    rect.bottom = pos.y + size.h;
    // TODO: remove hard-coded style parameter
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW ^ WS_OVERLAPPED, FALSE, NULL);
    return rect;
}

// TODO: improve key mapping performance
// Probably use two arrays instead of a map, barely any more memory, faster
// mapping
UINT Window::Impl::toWin32KeyCode(Key keyCode) {
    for (const auto& it : keyMappings)
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
Key Window::Impl::fromWin32KeyCode(UINT win32KeyCode) {
    auto it = keyMappings.find((int)win32KeyCode);
    return it == keyMappings.end() ? Key::Unknown : it->second;
}

LRESULT CALLBACK Window::Impl::wndProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
) {
    auto window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (uMsg) {

    // Signal close request
    case WM_CLOSE:
        window->impl->closeRequested = true;

        // Override default close behavior
        return 0;

    // Signal key character callback
    case WM_CHAR:
        {
            if (window->keyCharHandler) 
                window->keyCharHandler((char32_t)wParam);
        }
        break;

    // Signal cursor move callback
    // TODO: movement outside of client bounds should be detected, but isn't
    case WM_MOUSEMOVE:
        {
            CursorMoveEvent event;

            // Supplied coordinates are in client space
            POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            event.pos = { point.x, point.y };

            // Convert the point to screen space and store in event
            ClientToScreen(window->impl->hWnd, &point);
            event.screenPos = { point.x, point.y };
            
            if (window->cursorMoveHandler) window->cursorMoveHandler(event);
        }
        break;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        if (window->mouseButtonHandler) 
            window->mouseButtonHandler({
                MouseButton::LButton, 
                uMsg == WM_LBUTTONDOWN
            });
        break;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        if (window->mouseButtonHandler)
            window->mouseButtonHandler({
                MouseButton::RButton, 
                uMsg == WM_RBUTTONDOWN
            });
        break;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        if (window->mouseButtonHandler)
            window->mouseButtonHandler({
                MouseButton::MButton, 
                uMsg == WM_MBUTTONDOWN 
            });
        break;
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
        if (window->mouseButtonHandler)
            window->mouseButtonHandler({
                GET_XBUTTON_WPARAM(wParam) == XBUTTON1 
                    ? MouseButton::XButton1 
                    : MouseButton::XButton2, 
                uMsg == WM_XBUTTONDOWN
            }); 
        break;
    
    // For intercepting events such as keyboard input that give some strange 
    // undesired outputs using the normal window message
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

            switch (rawInput.header.dwType) {
            case RIM_TYPEKEYBOARD:
                {
                    // Skip unmapped keys
                    if (rawInput.data.keyboard.VKey == 0xFF) break;
                    
                    KeyEvent event;
                    event.down = !(rawInput.data.keyboard.Flags & RI_KEY_BREAK);
                    event.key = fromWin32KeyCode(
                        extractDiffWin32KeyCode(rawInput.data.keyboard)
                    );
                    if (window->keyHandler) window->keyHandler(event);
                }
                break;
            }
        }
        break;
    
    // Repaint the window white when it's been invalidated
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hWnd, &ps);
        }
        return 0;
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
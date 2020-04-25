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
#include <windows.h>
#include <winuser.h>
#include <windowsx.h>
#include <iostream>
#include <set>

using namespace esd::wnd;

Window::Window(
    std::string title, 
    WindowSize size, 
    std::optional<WindowPos> pos
) {

    impl = std::make_unique<Impl>();

    impl->hInstance = GetModuleHandleW(nullptr);
    
    constexpr wchar_t className[] = L"ESeed Window";
    
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = impl->hInstance;
    wc.hIcon = nullptr;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = className;
    wc.lpfnWndProc = (WNDPROC)Impl::wndProc;
    wc.cbWndExtra = sizeof(this);
    RegisterClassExW(&wc);

    RECT rect = impl->createWindowRect(size, *pos);
    impl->hWnd = CreateWindowExW(
        0,
        className,
        Impl::stringToWideString(title).c_str(),
        WS_OVERLAPPEDWINDOW,
        // Use automatic Win32 window placement if no position provided
        pos ? rect.left : CW_USEDEFAULT,
        pos ? rect.top : CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr,
        nullptr,
        impl->hInstance,
        nullptr
    );

    // Window is null if failed to initialize
    if (impl->hWnd == nullptr)
        throw std::runtime_error("Failed to create native Win32 window");

    // Set "this" pointer in window user data for use in WNDPROC
    SetWindowLongPtrW(impl->hWnd, GWLP_USERDATA, (LONG_PTR)this);

    ShowWindow(impl->hWnd, SW_SHOW);

    // Register raw input devices

    // Usage Reference:
    // https://www.usb.org/sites/default/files/documents/hut1_12v2.pdf

    RAWINPUTDEVICE rid[] = {
        { 0x01, 0x06, 0, impl->hWnd }, // Keyboard
        { 0x01, 0x07, 0, impl->hWnd }, // Keypad (Numpad)
    };

    RegisterRawInputDevices(rid, sizeof(rid) / sizeof(rid[0]), sizeof(rid[0]));
}

Window::~Window() {
    if (impl->hWnd) close();
}

void Window::close() {
    DestroyWindow(impl->hWnd);
    impl->hWnd = nullptr;
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
    wchar_t* cwtitle = new wchar_t[length + 1];
    GetWindowTextW(impl->hWnd, cwtitle, length + 1);

    std::string title = Impl::wideStringToString(std::wstring(cwtitle));

    delete[] cwtitle;
    
    return title;
}

void Window::setTitle(std::string title) {
    SetWindowTextW(impl->hWnd, Impl::stringToWideString(title).c_str());
}

WindowSize Window::getSize() {
    RECT rect; 
    GetClientRect(impl->hWnd, &rect);
    return WindowSize { 
        static_cast<int>(rect.right - rect.left), 
        static_cast<int>(rect.bottom - rect.top) 
    };
}

void Window::setSize(WindowSize size) {

    if (isFullscreen()) return;
    
    RECT rect = impl->createWindowRect(size, getPos());

    int diffX = static_cast<int>(size.w) - rect.right - rect.left;
    int diffY = static_cast<int>(size.h) - rect.bottom - rect.top;

    SetWindowPos(
        impl->hWnd, 
        nullptr,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        0
    );
}

WindowPos Window::getPos() {
    RECT rect = {};
    ClientToScreen(impl->hWnd, reinterpret_cast<LPPOINT>(&rect));
    return { rect.left, rect.top };
}

void Window::setPos(WindowPos pos) {
    
    if (isFullscreen()) return;

    RECT rect = impl->createWindowRect(getSize(), pos);
    
    SetWindowPos(
        impl->hWnd, 
        nullptr,
        rect.left,
        rect.top,
        rect.right - rect.left,
        rect.bottom - rect.top,
        0
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
            nullptr,
            0, 0, 0, 0,
            SWP_NOMOVE
                | SWP_NOSIZE
                | SWP_NOZORDER
                | SWP_NOOWNERZORDER
                | SWP_FRAMECHANGED
        );
    }

    InvalidateRect(impl->hWnd, nullptr, TRUE);
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
    throw std::runtime_error("Unknown key");
}

CursorPos Window::getCursorPos() {
    POINT point;
    GetCursorPos(&point);
    ScreenToClient(impl->hWnd, &point);
    return CursorPos { static_cast<double>(point.x), static_cast<double>(point.y) };
}

void Window::setCursorPos(CursorPos pos) {
    POINT point = { static_cast<LONG>(pos.x), static_cast<LONG>(pos.y) };
    ClientToScreen(impl->hWnd, &point);
    SetCursorPos(point.x, point.y);
}

CursorPos Window::getCursorScreenPos() {
    POINT point;
    GetCursorPos(&point);
    return CursorPos { static_cast<double>(point.x), static_cast<double>(point.y) };
}

void Window::setCursorScreenPos(CursorPos pos) {
    SetCursorPos(static_cast<LONG>(pos.x), static_cast<LONG>(pos.y));
}

bool Window::isMouseButtonDown(MouseButton button) {
    for (auto it : mouseButtonMappings) {
        if (it.second == button) return GetKeyState(it.first) & 0x8000;
    }
    throw std::runtime_error("Unknown mouse button");
}

RECT Window::Impl::createWindowRect(WindowSize size, WindowPos pos) {
    RECT rect;
    rect.left = pos.x;
    rect.top = pos.y;
    rect.right = pos.x + size.w;
    rect.bottom = pos.y + size.h;
    // TODO: remove hard-coded style parameter
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW ^ WS_OVERLAPPED, FALSE, 0);
    return rect;
}

// TODO: improve key mapping performance
// Probably use two arrays instead of a map, barely any more memory, faster 
// mapping
UINT Window::Impl::toWin32KeyCode(Key keyCode) {
    for (const auto& it : keyMappings)
        if (it.second == keyCode) return it.first;
    return 0;
}

UINT Window::Impl::extractDiffWin32KeyCode(const RAWKEYBOARD& rawKeyboard) {
    // Differentiate left and right modifier keys
    bool extended = rawKeyboard.Flags & RI_KEY_E0;
    switch (rawKeyboard.VKey) {
    case VK_SHIFT: return MapVirtualKeyW(rawKeyboard.MakeCode, MAPVK_VSC_TO_VK_EX);
    case VK_CONTROL: return extended ? VK_RCONTROL : VK_LCONTROL;
    case VK_MENU: return extended ? VK_RMENU : VK_LMENU;
    default: return rawKeyboard.VKey;
    }
}

// TODO: improve key mapping performance
Key Window::Impl::fromWin32KeyCode(UINT win32KeyCode) {
    auto it = keyMappings.find(win32KeyCode);
    return it == keyMappings.end() ? Key::Unknown : it->second;
}

LRESULT CALLBACK Window::Impl::wndProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
) {
    auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (uMsg) {

    case WM_CLOSE:
        window->impl->closeRequested = true;
        return 0;

    case WM_SIZE:
        if (window->resizeHandler) {
            ResizeEvent event;
            event.size = { LOWORD(lParam), HIWORD(lParam) };
            window->resizeHandler(event);
        }

        return 0;

    case WM_MOVE:
        if (window->moveHandler) {
            MoveEvent event;
            event.pos = { static_cast<short>(LOWORD(lParam)), static_cast<short>(HIWORD(lParam)) };
            window->moveHandler(event);
        }

        return 0;

    case WM_CHAR:
        if (window->keyCharHandler) {
            KeyCharEvent event;
            event.codePoint = static_cast<char32_t>(wParam);
            window->keyCharHandler(event);
        }
        
        return 0;

    case WM_MOUSEMOVE:
        if (window->cursorMoveHandler) {
            CursorMoveEvent event;

            // Supplied coordinates are in client space
            POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            event.pos = { static_cast<double>(point.x), static_cast<double>(point.y) };

            // Convert the point to screen space and store in event
            ClientToScreen(window->impl->hWnd, &point);
            event.screenPos = { static_cast<double>(point.x), static_cast<double>(point.y) };

            // If the cursor was previously out of the window, it just entered
            event.entered = !window->impl->cursorInWindow;
            
            window->cursorMoveHandler(event);
        }

        // If cursor just entered the window, start tracking for next exit
        if (!window->impl->cursorInWindow) {
            window->impl->cursorInWindow = true;

            TRACKMOUSEEVENT tme;
            tme.cbSize = sizeof(tme);
            tme.hwndTrack = window->impl->hWnd;
            tme.dwFlags = TME_LEAVE;
            
            TrackMouseEvent(&tme);
        }
        
        return 0;
    
    case WM_MOUSELEAVE:
        window->impl->cursorInWindow = false;
        if (window->cursorExitHandler) {
            window->cursorExitHandler({});
        }

        return 0;
    
    case WM_MOUSEWHEEL:
        if (window->scrollHandler) {
            ScrollEvent event;
            event.vScroll = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
            event.hScroll = 0;
            window->scrollHandler(event);
        }

        return 0;

    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
        if (window->mouseButtonHandler) 
            window->mouseButtonHandler({
                MouseButton::LButton, 
                uMsg == WM_LBUTTONDOWN
            });
        return 0;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
        if (window->mouseButtonHandler)
            window->mouseButtonHandler({
                MouseButton::RButton, 
                uMsg == WM_RBUTTONDOWN
            });
        return 0;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
        if (window->mouseButtonHandler)
            window->mouseButtonHandler({
                MouseButton::MButton, 
                uMsg == WM_MBUTTONDOWN 
            });
        return 0;
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
        if (window->mouseButtonHandler)
            window->mouseButtonHandler({
                GET_XBUTTON_WPARAM(wParam) == XBUTTON1 
                    ? MouseButton::XButton1
                    : MouseButton::XButton2, 
                uMsg == WM_XBUTTONDOWN
            }); 
        return 0;
    
    // For intercepting events such as keyboard input that give some strange 
    // undesired outputs using the normal window message
    case WM_INPUT:
        {
            RAWINPUT rawInput;
            UINT rawInputSize = sizeof(rawInput);

            GetRawInputData(
                reinterpret_cast<HRAWINPUT>(lParam), 
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
        0, 
        reinterpret_cast<LPCWCH>(wstring.c_str()),
        -1, 
        nullptr, 
        0,
        nullptr, 
        nullptr
    );

    char* cstring = new char[length];

    WideCharToMultiByte(
        CP_UTF8,
        0,
        reinterpret_cast<LPCWCH>(wstring.c_str()),
        -1,
        reinterpret_cast<LPSTR>(cstring),
        length,
        nullptr,
        nullptr
    );

    std::string string(cstring);

    delete[] cstring;

    return string;
}

std::wstring Window::Impl::stringToWideString(const std::string& string) {
    int wlength = MultiByteToWideChar(
        CP_UTF8,
        0,
        reinterpret_cast<LPCCH>(string.c_str()),
        -1,
        nullptr,
        0
    );

    wchar_t* cwstring = new wchar_t[wlength];

    MultiByteToWideChar(
        CP_UTF8,
        0,
        reinterpret_cast<LPCCH>(string.c_str()),
        -1,
        reinterpret_cast<LPWSTR>(cwstring),
        wlength
    );

    std::wstring wstring(cwstring);

    delete[] cwstring;

    return wstring;
}
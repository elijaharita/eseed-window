#include <eseed/window/window.hpp>

#include "keycodemappings.hpp"
#include <Windows.h>
#include <iostream>
#include <set>

using namespace esd::window;

class Window::Impl {
public:
    HINSTANCE hInstance;
    HWND hWnd;
    bool closeRequested;

    static KeyCode fromWin32KeyCode(WPARAM wParam, LPARAM lParam);
    static int toWin32KeyCode(KeyCode keyCode);

    // Win32 WNDPROC
    static LRESULT CALLBACK wndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
    );
    
    static RECT createWindowRect(Size size);
};

Window::Window(std::string title, Size size) {

    impl = std::make_unique<Impl>();

    impl->hInstance = GetModuleHandleA(NULL);
    
    constexpr char className[] = "ESeed Window";
    
    WNDCLASSEXA wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.hInstance = impl->hInstance;
    wc.hIcon = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = className;
    wc.lpfnWndProc = (WNDPROC)Impl::wndProc;
    wc.cbWndExtra = sizeof(Window*);
    RegisterClassExA(&wc);


    RECT rect = Impl::createWindowRect(size);
    impl->hWnd = CreateWindowExA(
        NULL,
        className,
        title.c_str(),
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

    if (impl->hWnd == NULL)
        throw std::runtime_error("Failed to create native Win32 window");

    SetWindowLongPtrA(impl->hWnd, GWLP_USERDATA, (LONG_PTR)this);
    ShowWindow(impl->hWnd, SW_SHOW);
}

Window::~Window() {
    if (impl->hWnd) DestroyWindow(impl->hWnd);
}

void Window::poll() {
    MSG msg;
    while (PeekMessageA(&msg, impl->hWnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}

Size Window::getSize() {
    return { 1, 1 };
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

// TODO: improve key mapping performance
int Window::Impl::toWin32KeyCode(KeyCode keyCode) {
    for (const auto& it : keyCodeMappings)
        if (it.second == keyCode) return it.first;
    return NULL;
}

// TODO: improve key mapping performance
KeyCode Window::Impl::fromWin32KeyCode(WPARAM wParam, LPARAM lParam) {

    // Differentiate left and right modifier keys
    bool extended = lParam & 0x01000000;
    switch (wParam) {
    case VK_SHIFT:
        wParam = 
            MapVirtualKeyA((lParam & 0x00ff0000) >> 16, MAPVK_VSC_TO_VK_EX);
        break;
    case VK_CONTROL:
        wParam = extended ? VK_RCONTROL : VK_LCONTROL;
        break;
    case VK_MENU:
        wParam = extended ? VK_RMENU : VK_LMENU;
        break;
    }

    try {
        return keyCodeMappings.at((int)wParam);
    } catch (std::out_of_range&) {
        return KeyCode::Unknown;
    }
}

LRESULT CALLBACK Window::Impl::wndProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
) {
    auto window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (uMsg) {
    case WM_CLOSE:
        window->impl->closeRequested = true;
        return NULL;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        {
            KeyEvent event;
            event.down = !(HIWORD(lParam) & KF_UP);
            event.keyCode = fromWin32KeyCode(wParam, lParam);
            if (window->keyHandler) window->keyHandler(event);
        }
        return NULL;
    }

    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
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
#include <eseed/window/window.hpp>

#include <Windows.h>
#include <iostream>

using namespace esd::window;

class Window::Impl {
public:
    HINSTANCE hInstance;
    HWND hWnd;
    bool closeRequested;

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

void Window::setSize(Size size) {
    RECT rect;
    AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW ^ WS_OVERLAPPED, FALSE, NULL);
}

bool Window::isCloseRequested() {
    return impl->closeRequested;
}

LRESULT CALLBACK Window::Impl::wndProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
) {
    auto window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (uMsg) {
    case WM_CLOSE:
        window->impl->closeRequested = true;
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
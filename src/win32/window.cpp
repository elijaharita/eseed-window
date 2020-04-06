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
    
    // Create a Win32 RECT adjusted for the window style based on an
    // eseed::window Size
    static RECT createWindowRect(Size size);

    // Get VKey from a Win32 RAWKEYBOARD with differentiated left and right
    // modifier keys (e.g. VK_LSHIFT and VK_RSHIFT instead of VK_SHIFT)
    static UINT extractDiffWin32KeyCode(const RAWKEYBOARD& rawKeyboard);

    // Convert a Win32 virtual key code to esd::window key code
    static KeyCode fromWin32KeyCode(UINT win32KeyCode);

    // Convert an esd::window key code to Win32 virtual key code
    static UINT toWin32KeyCode(KeyCode keyCode);

    // Win32 WNDPROC
    static LRESULT CALLBACK wndProc(
        HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam
    );

    // Win32 HOOKPROC
    static LRESULT CALLBACK lowLevelKeyboardProc(
        int nCode, WPARAM wParam, LPARAM lParam
    );
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
    while (PeekMessageA(&msg, impl->hWnd, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
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
        return MapVirtualKeyA(rawKeyboard.MakeCode, MAPVK_VSC_TO_VK_EX);
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
        return NULL;
    case WM_CHAR:
        {
            if (window->keyCharHandler) window->keyCharHandler((char)wParam);
        }
        break;
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

    return DefWindowProcA(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK Window::Impl::lowLevelKeyboardProc(
    int nCode, WPARAM wParam, LPARAM lParam
) {
    return nCode;
}
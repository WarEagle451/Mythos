#include <mythos/platform/windows/win_window.hpp>
#ifdef MYL_OS_WINDOWS
#   include <mythos/core/application.hpp>
#   include <mythos/log.hpp>
#   include <mythos/platform/windows/win_utilities.hpp>

#	include <WinUser.h>

///BUG: Somewhere in here causes a bad termination

namespace myth::win {
    MYL_NO_DISCARD window::window()
        : myth::window() {

        constexpr const char* class_name = "mythos_window";

        //// Setup and register window class
        m_instance = GetModuleHandleA(0);
        WNDCLASSA window_class{
            .style = CS_DBLCLKS,
            .lpfnWndProc = process_message,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = m_instance,
            .hIcon = LoadIcon(m_instance, IDI_APPLICATION),
            .hCursor = LoadCursor(NULL, IDC_ARROW),
            .hbrBackground = NULL,
            .lpszMenuName = NULL,
            .lpszClassName = class_name
        };

        if (!RegisterClassA(&window_class))
            MYTHOS_FATAL("Window creation failed: {}", win::last_error_as_string(GetLastError()));

        DWORD window_style = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPED | WS_THICKFRAME;
        myl::u32 window_ex_style = WS_EX_APPWINDOW;

        int w = 800;
        int h = 600;
        int x = (GetSystemMetrics(SM_CXSCREEN) - w) / 2;
        int y = (GetSystemMetrics(SM_CYSCREEN) - h) / 2;

        RECT window_rect = { 0, 0, 0, 0 };
		AdjustWindowRectEx(&window_rect, window_style, FALSE, window_ex_style);

        m_handle = CreateWindowExA(window_ex_style, class_name, "Add Title", window_style, x, y, w, h, NULL, NULL, m_instance, NULL);
        if (!m_handle)
            MYTHOS_FATAL("Window creation failed: {}", win::last_error_as_string(GetLastError()));

        ShowWindow(m_handle, SW_SHOWNORMAL);
    }

    window::~window() {
        if (m_handle)
            DestroyWindow(m_handle);
    }

    auto window::update() -> void {
        MSG msg;
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    auto CALLBACK window::process_message(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) -> LRESULT {
        win::window* target = static_cast<win::window*>(application::get().main_window());
        switch (msg) {
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }

        return DefWindowProc(hwnd, msg, w_param, l_param);
    }
}
#endif

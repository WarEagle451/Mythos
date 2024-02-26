#include <mythos/platform/windows/win_window.hpp>
#ifdef MYL_OS_WINDOWS
#   include <mythos/core/application.hpp>
#   include <mythos/log.hpp>
#   include <mythos/platform/windows/win_utilities.hpp>

#    include <WinUser.h>

///BUG: Somewhere in here causes a bad termination

namespace myth::win {
    MYL_NO_DISCARD static auto client_dimensions_to_window_dimensions(HWND handle, const myl::i32vec2& client_dimensions) -> myl::i32vec2 {
        RECT cr{}, wr{};
        GetClientRect(handle, &cr);
        GetWindowRect(handle, &wr);
        LONG bw = wr.right - wr.left - cr.right;
        LONG bh = wr.bottom - wr.top - cr.bottom;
        return { client_dimensions.w + bw, client_dimensions.h + bh };
    }

    MYL_NO_DISCARD static auto correct_negative_position(const myl::i32vec2& position, const myl::i32vec2& dimensions) -> myl::i32vec2 {
        return { // Negative position values are illformed, the window will be created in the center of the monitor on that axis
            position.x < 0 ? (GetSystemMetrics(SM_CXSCREEN) - dimensions.w) / 2 : position.x,
            position.y < 0 ? (GetSystemMetrics(SM_CYSCREEN) - dimensions.h) / 2 : position.y
        };
    }

    MYL_NO_DISCARD window::window(const window_configuration& config)
        : myth::window(config) {
        constexpr const char* class_name = "mythos_window";
    
        // Setup and register window class
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
    
        int x{}, y{}, w{}, h{};
        { ///TEMP: This is good for non fullscreen
            RECT window_rect = { 0, 0, 0, 0 };
            AdjustWindowRectEx(&window_rect, window_style, FALSE, window_ex_style);

            m_position = correct_negative_position(m_position, m_dimensions);
    
            // Client area position - border rect. (In this case, the border rect is negative)
            x = m_position.x - window_rect.left;
            y = m_position.y - window_rect.top;
    
            // Client size + the size of OS bordering
            w = m_dimensions.w + window_rect.right - window_rect.left;
            h = m_dimensions.h + window_rect.bottom - window_rect.top;
        }
    
        m_handle = CreateWindowExA(window_ex_style, class_name, m_title, window_style, x, y, w, h, NULL, NULL, m_instance, NULL);
        if (!m_handle)
            MYTHOS_FATAL("Window creation failed: {}", win::last_error_as_string(GetLastError()));
        
        // Ensures set_state will not immediately reeturn
        m_state = window_state::unknown;
        ///TODO: Can I window be initialized as minimized? Will the renderer allow this?
        set_state(config.state);
        
        /// Initial ShowWindow does not send WM_SIZE because events are not working, this should be done earlier
        if (m_state == window_state::maximized) {
            RECT client{};
            GetClientRect(m_handle, &client);
            m_dimensions.x = client.right;
            m_dimensions.y = client.bottom;
        }
    }

    window::~window() {
        if (m_handle)
            DestroyWindow(m_handle);
    }

    auto window::set_title(const char* title) -> void {
        if (!SetWindowTextA(m_handle, title))
            MYTHOS_ERROR("Window retitle failed: {}", win::last_error_as_string(GetLastError()));
        m_title = title;
    }

    auto window::set_state(window_state state) -> void {
        if (state == m_state)
            return;
            
        switch (state) {
            using enum window_state;
            case unknown:
                MYTHOS_WARN("Unknown window state, maintaining currrent state");
                return;
            case normal:
                ShowWindow(m_handle, SW_SHOWNORMAL);
                break;
            case maximized:
                ShowWindow(m_handle, SW_SHOWMAXIMIZED);
                break;
            case minimized:
                CloseWindow(m_handle);
                break;
            default:
                MYTHOS_ERROR("Unhandled window state, maintaining currrent state");
                return;
        }

        m_state = state;
    }

    auto window::set_position(const myl::i32vec2& position) -> void {
        auto pos = correct_negative_position(position, m_dimensions);
        // Sends WM_MOVE and therefore sets position
        if (!SetWindowPos(m_handle, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE))
            MYTHOS_ERROR("Window set position failed: {}", win::last_error_as_string(GetLastError()));
        else
            m_position = pos;
    }

    auto window::set_dimensions(const myl::i32vec2& dimensions) -> void {
        auto dim = client_dimensions_to_window_dimensions(m_handle, dimensions);
        // Sends WM_SIZE and therefore calls on_resize
        if (!SetWindowPos(m_handle, NULL, 0, 0, dim.w, dim.h, SWP_NOMOVE))
            MYTHOS_ERROR("Window set dimensions failed: {}", win::last_error_as_string(GetLastError()));
        else
            m_dimensions = dim;
    }

    auto window::update() -> void {
        MSG msg;
        while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageA(&msg);
        }
    }

    // Unimplemented (possibly useful) messages
    // - WM_ENTERSIZEMOVE, WM_EXITSIZEMOVE
    // - WM_INPUT
    // - WM_CREATE, WM_NCCREATE, WM_NCDESTROY
    // - WM_SHOWWINDOW, WM_ACTIVATEAPP, WM_ACTIVATE, WM_NCACTIVATE
    // - WM_ENABLE
    // - WM_QUERYOPEN
    // - WM_TIMECHANGE
    // - WM_CANCELMODE
    // - WM_SETCURSOR
    // - WM_MOUSEACTIVATE
    // - WM_GETMINMAXINFO
    // - WM_POWER - Could be part of the solution to suspend the app's time loop
    // - WM_SPOOLERSTATUS
    // - WM_SETHOTKEY, WM_GETHOTKEY
    // - WM_QUERYDRAGICON
    // - WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED - vs WM_MOVE ?
    // - WM_COPYDATA
    // - WM_INPUTLANGCHANGEREQUEST, WM_INPUTLANGCHANGE
    // - WM_USERCHANGED, WM_USER
    // - WM_STYLECHANGING, WM_STYLECHANGED
    // - WM_DISPLAYCHANGE
    // - WM_NCCALCSIZE
    // - WM_GETDLGCODE
    // - WM_NCMOUSEMOVE, WM_MOUSEMOVE
    // - WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK, WM_RBUTTONDOWN, WM_RBUTTONUP, WM_RBUTTONDBLCLK, WM_MBUTTONDOWN, WM_MBUTTONUP, WM_MBUTTONDBLCLK, WM_XBUTTONDOWN, WM_XBUTTONUP, WM_XBUTTONDBLCLK
    // - WM_NCLBUTTONDOWN, WM_NCLBUTTONUP, WM_NCLBUTTONUP, WM_NCLBUTTONDBLCLK, WM_NCRBUTTONDOWN, WM_NCRBUTTONUP, WM_NCRBUTTONDBLCLK, WM_NCMBUTTONDOWN, WM_NCMBUTTONUP, WM_NCMBUTTONDBLCLK, WM_NCXBUTTONDOWN, WM_NCXBUTTONUP, WM_NCXBUTTONDBLCLK
    // - WM_KEYDOWN, WM_KEYFIRST, WM_KEYUP, WM_CHAR, WM_DEADCHAR, WM_UNICHAR / WM_KEYLAST
    // - WM_SYSKEYDOWN, WM_SYSKEYUP, WM_SYSCHAR, WM_SYSDEADCHAR
    // - WM_APP
    // - WM_CAP_SET_CALLBACK_ERRORW, WM_CAP_SET_CALLBACK_STATUSW, WM_CAP_DRIVER_GET_NAMEW, WM_CAP_DRIVER_GET_VERSIONW, WM_CAP_FILE_SET_CAPTURE_FILEW, WM_CAP_FILE_GET_CAPTURE_FILEW, WM_CAP_FILE_SAVEASW, WM_CAP_SET_MCI_DEVICEW, WM_CAP_GET_MCI_DEVICEW, WM_CAP_PAL_OPENW, WM_CAP_PAL_SAVEW, WM_CAP_FILE_SAVEDIBW
    // - WM_HSCROLL, WM_VSCROLL, WM_MOUSEWHEEL, WM_MOUSEHWHEEL
    // - WM_MOUSEFIRST, WM_MOUSELAST
    // - WM_CUT, WM_COPY, WM_PASTE, WM_CLEAR, WM_UNDO
    // - WM_SIZING, WM_MOVING
    // - WM_NCMOUSEHOVER, WM_MOUSEHOVER
    // - WM_NCMOUSELEAVE, WM_MOUSELEAVE
    // - DL_BEGINDRAG, DL_CANCELDRAG, DL_DRAGGING, DL_DROPPED
    // - HKM_GETHOTKEY, HKM_SETHOTKEY, HKM_SETRULES
    // - WM_CAPTURECHANGED
    // - WM_POWERBROADCAST
    // - WM_DEVICECHANGE
    // - WM_DROPFILES
    // - WM_RENDERFORMAT, WM_RENDERALLFORMATS, WM_DESTROYCLIPBOARD, WM_DRAWCLIPBOARD, WM_PAINTCLIPBOARD, WM_VSCROLLCLIPBOARD, WM_SIZECLIPBOARD, WM_ASKCBFORMATNAME, WM_CHANGECBCHAIN, WM_HSCROLLCLIPBOARD
    // - WM_QUERYNEWPALETTE, WM_PALETTEISCHANGING, WM_PALETTECHANGED
    // - WM_HOTKEY
    // - WM_PRINT, WM_PRINTCLIENT
    // - WM_APPCOMMAND
    // - WM_HANDHELDFIRST, WM_HANDHELDLAST
    // - WM_DPICHANGED
    
    auto CALLBACK window::process_message(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) -> LRESULT {
        win::window* target = static_cast<win::window*>(application::get().main_window()); 
        if (target)
            switch (msg) {
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
                case WM_MOVE:
                    target->m_position = { LOWORD(l_param), HIWORD(l_param) };
                    break;
                case WM_SIZE: {
                    ///TODO: Should not be setting targets m_state, should be able passing it to event
                    switch (w_param) {
                        case SIZE_RESTORED:  target->m_state = window_state::normal; break;
                        case SIZE_MINIMIZED: target->m_state = window_state::minimized; break;
                        case SIZE_MAXSHOW:   MYTHOS_WARN("WM_SIZE value 'SIZE_MAXSHOW' not implemented"); return 0;
                        case SIZE_MAXIMIZED: target->m_state = window_state::maximized; break;
                        case SIZE_MAXHIDE:   MYTHOS_WARN("WM_SIZE value 'SIZE_MAXHIDE' not implemented"); return 0;
                        default:
                            MYTHOS_WARN("Unhandled WM_SIZE WPARAM: {}", w_param);
                            return DefWindowProcA(hwnd, msg, w_param, l_param);
                    }
                
                    event::window_resize e(*target, LOWORD(l_param), HIWORD(l_param));
                    event::fire(e);
                    break;
                }
                case WM_SETFOCUS: {
                    /// wParam contains: A handle to the window that has lost the keyboard focus. This parameter can be NULL.
                    event::window_focus_gain e(*target);
                    event::fire(e);
                    return 0;
                }
                case WM_KILLFOCUS: {
                    /// wParam contains: A handle to the window that receives the keyboard focus. This parameter can be NULL.
                    event::window_focus_lost e(*target);
                    event::fire(e);
                    return 0;
                }
                case WM_CLOSE: {
                    event::window_close e(*target);
                    event::fire(e);
                    return 0;
                }
                case WM_ERASEBKGND: // Notify the OS that erasing will be handled by the app to prevent flicker
					return 1;
            }

        return DefWindowProcA(hwnd, msg, w_param, l_param);
    }
}
#endif

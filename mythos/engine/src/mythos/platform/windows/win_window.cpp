#include <mythos/platform/windows/win_window.hpp>
#ifdef MYL_OS_WINDOWS
#   include <mythos/core/application.hpp>
#   include <mythos/core/keyboard.hpp>
#   include <mythos/input.hpp>
#   include <mythos/log.hpp>
#   include <mythos/platform/windows/win_utilities.hpp>

#   include <Windows.h> // WinUser.h
#	include <hidusage.h>

// https://learn.microsoft.com/en-us/windows/win32/winmsg/window-styles

namespace myth::win {
    static auto register_for_raw_input_devices(HWND window_handle) -> void {
        constexpr UINT device_type_count = 4;
        RAWINPUTDEVICE devices[device_type_count]{
            { // Mouse
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_MOUSE,
                //.dwFlags = RIDEV_NOLEGACY, // RIDEV_CAPTUREMOUSE; Will mouse buttons will not activate other windows, but how to toggle this?
                .hwndTarget = window_handle
            },
            { // Joy Stick
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_JOYSTICK,
                //.dwFlags = RIDEV_NOLEGACY,
                .hwndTarget = window_handle
            },
            { // Gamepad
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_GAMEPAD,
                //.dwFlags = RIDEV_NOLEGACY,
                .hwndTarget = window_handle
            },
            { // Keyboard
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_KEYBOARD,
                ///MYBUG: RIDEV_NOLEGACY blocks WM_CHAR messages, an alternative method of sending typed events must be implemented,
                /// maybe process_typed after process_key in RIM_TYPEKEYBOARD if state == to down
                //.dwFlags = RIDEV_NOLEGACY,
                .hwndTarget = window_handle
            }
        };

        if (!RegisterRawInputDevices(devices, device_type_count, sizeof(RAWINPUTDEVICE)))
            MYTHOS_FATAL("Failed to register raw input devices: {}", win::last_error_as_string(GetLastError()));
    }

    static auto unregister_raw_input_devices() -> void {
        // Microsoft docs state that hwndTarget must be NULL if RIDEV_REMOVE is set or RegisterRawInputDevices will fail
        // See Remarks: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawinputdevice#remarks

        constexpr UINT device_type_count = 4;
        RAWINPUTDEVICE devices[device_type_count]{
            { // Mouse
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_MOUSE,
                .dwFlags = RIDEV_REMOVE,
                .hwndTarget = NULL
            },
            { // Joy Stick
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_JOYSTICK,
                .dwFlags = RIDEV_REMOVE,
                .hwndTarget = NULL
            },
            { // Gamepad
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_GAMEPAD,
                .dwFlags = RIDEV_REMOVE,
                .hwndTarget = NULL
            },
            { // Keyboard
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_KEYBOARD,
                .dwFlags = RIDEV_REMOVE,
                .hwndTarget = NULL
            }
        };

        if (!RegisterRawInputDevices(devices, device_type_count, sizeof(RAWINPUTDEVICE)))
            MYTHOS_ERROR("Failed to unregister raw input devices: {}", win::last_error_as_string(GetLastError()));
    }

    MYL_NO_DISCARD static auto client_dimensions_to_window_dimensions(HWND handle, const myl::i32vec2& client_dimensions) -> myl::i32vec2 {
        RECT cr{}, wr{};
        GetClientRect(handle, &cr);
        GetWindowRect(handle, &wr);
        LONG bw = wr.right - wr.left - cr.right;
        LONG bh = wr.bottom - wr.top - cr.bottom;
        return myl::i32vec2{ client_dimensions.w + bw, client_dimensions.h + bh };
    }

    MYL_NO_DISCARD static auto correct_negative_position(const myl::i32vec2& position, const myl::i32vec2& dimensions) -> myl::i32vec2 {
        return myl::i32vec2{ // Negative position values are illformed, the window will be created in the center of the monitor on that axis
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
        
        m_state = window_state::unknown; // Ensures set_state will not immediately return
        ///MYTODO: Can I window be initialized as minimized? Will the renderer allow this?
        set_state(config.state);
        
        /// Initial ShowWindow does not send WM_SIZE because events are not working, this should be done earlier
        if (m_state == window_state::maximized) {
            RECT client{};
            GetClientRect(m_handle, &client);
            m_dimensions.x = client.right;
            m_dimensions.y = client.bottom;
        }

        register_for_raw_input_devices(m_handle);
    }

    window::~window() {
        unregister_raw_input_devices();

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
        // Sends WM_MOVE and therefore sets m_position in WM_MOVE
        if (!SetWindowPos(m_handle, NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE))
            MYTHOS_ERROR("Window set position failed: {}", win::last_error_as_string(GetLastError()));
    }

    auto window::set_dimensions(const myl::i32vec2& dimensions) -> void {
        auto dim = client_dimensions_to_window_dimensions(m_handle, dimensions);
        // Sends WM_SIZE and therefore sets m_dimensions in WM_SIZE
        if (!SetWindowPos(m_handle, NULL, 0, 0, dim.w, dim.h, SWP_NOMOVE))
            MYTHOS_ERROR("Window set dimensions failed: {}", win::last_error_as_string(GetLastError()));
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
    // - WM_CREATE, WM_NCCREATE, WM_NCDESTROY
    // - WM_SHOWWINDOW, WM_ACTIVATEAPP, WM_ACTIVATE, WM_NCACTIVATE
    // - WM_ENABLE
    // - WM_QUERYOPEN
    // - WM_TIMECHANGE
    // - WM_CANCELMODE
    // - WM_SETCURSOR
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
    // - WM_KEYDOWN, WM_KEYFIRST, WM_KEYUP, WM_DEADCHAR, WM_UNICHAR / WM_KEYLAST
    // - WM_SYSKEYDOWN, WM_SYSKEYUP, WM_SYSCHAR, WM_SYSDEADCHAR
    // - WM_APP
    // - WM_CAP_SET_CALLBACK_ERRORW, WM_CAP_SET_CALLBACK_STATUSW, WM_CAP_DRIVER_GET_NAMEW, WM_CAP_DRIVER_GET_VERSIONW, WM_CAP_FILE_SET_CAPTURE_FILEW, WM_CAP_FILE_GET_CAPTURE_FILEW, WM_CAP_FILE_SAVEASW, WM_CAP_SET_MCI_DEVICEW, WM_CAP_GET_MCI_DEVICEW, WM_CAP_PAL_OPENW, WM_CAP_PAL_SAVEW, WM_CAP_FILE_SAVEDIBW
    // - WM_HSCROLL, WM_VSCROLL
    // - WM_MOUSEFIRST, WM_MOUSELAST
    // - WM_CUT, WM_COPY, WM_PASTE, WM_CLEAR, WM_UNDO
    // - WM_SIZING, WM_MOVING
    // - DL_BEGINDRAG, DL_CANCELDRAG, DL_DRAGGING, DL_DROPPED
    // - HKM_GETHOTKEY, HKM_SETHOTKEY, HKM_SETRULES
    // - WM_POWERBROADCAST
    // - WM_DROPFILES
    // - WM_RENDERFORMAT, WM_RENDERALLFORMATS, WM_DESTROYCLIPBOARD, WM_DRAWCLIPBOARD, WM_PAINTCLIPBOARD, WM_VSCROLLCLIPBOARD, WM_SIZECLIPBOARD, WM_ASKCBFORMATNAME, WM_CHANGECBCHAIN, WM_HSCROLLCLIPBOARD
    // - WM_QUERYNEWPALETTE, WM_PALETTEISCHANGING, WM_PALETTECHANGED
    // - WM_HOTKEY
    // - WM_PRINT, WM_PRINTCLIENT
    // - WM_APPCOMMAND
    // - WM_HANDHELDFIRST, WM_HANDHELDLAST
    // - WM_DPICHANGED
    // - WM_DEVICECHANGE
    
    // Using WM_INPUT and RIDEV_NOLEGACY disables the following messages;
    // WM_CAPTURECHANGED, WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK, WM_RBUTTONDOWN, WM_RBUTTONUP, WM_RBUTTONDBLCLK, WM_MBUTTONDOWN, WM_MBUTTONUP, WM_MBUTTONDBLCLK, WM_XBUTTONDOWN, WM_XBUTTONUP,
    // WM_XBUTTONDBLCLK, WM_MOUSEACTIVATE, WM_MOUSEHOVER, WM_MOUSEWHEEL, WM_MOUSEHWHEEL, WM_MOUSELEAVE, WM_MOUSEMOVE, WM_NCLBUTTONDOWN, WM_NCLBUTTONUP, WM_NCLBUTTONUP, WM_NCLBUTTONDBLCLK, WM_NCRBUTTONDOWN,
    // WM_NCRBUTTONUP, WM_NCRBUTTONDBLCLK, WM_NCMBUTTONDOWN, WM_NCMBUTTONUP, WM_NCMBUTTONDBLCLK, WM_NCXBUTTONDOWN, WM_NCXBUTTONUP, WM_NCXBUTTONDBLCLK, WM_NCMOUSEMOVE, WM_NCMOUSEHOVER, WM_NCMOUSELEAVE
    // Minic behaviour of them

    auto CALLBACK window::process_message(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) -> LRESULT {
        win::window* target = static_cast<win::window*>(application::get().main_window()); 
        if (target)
            switch (msg) {
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
                case WM_MOVE:
                    target->m_position = { static_cast<myl::i32>(LOWORD(l_param)), static_cast<myl::i32>(HIWORD(l_param)) };
                    break;
                case WM_SIZE: {
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
                    target->m_dimensions = { static_cast<myl::i32>(LOWORD(l_param)), static_cast<myl::i32>(HIWORD(l_param)) };
                
                    event::window_resize e(*target, target->m_dimensions);
                    event::fire(e);
                    break;
                }
                case WM_SETFOCUS: {
                    ///MYTODO: wParam contains: A handle to the window that has lost the keyboard focus. This parameter can be NULL.
                    event::window_focus_gain e(*target);
                    event::fire(e);
                    return 0;
                }
                case WM_KILLFOCUS: {
                    ///MYTODO: wParam contains: A handle to the window that receives the keyboard focus. This parameter can be NULL.
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
                case WM_INPUT: {
                    ///MYTODO: Possibly relavent https://devblogs.microsoft.com/oldnewthing/20160627-00/?p=93755#:~:text=Finally%2C%20we%20call-,Def%C2%ADRaw%C2%ADInput%C2%ADProc,-to%20allow%20default
                    
                    RAWINPUT raw{};
                    UINT pcb_size = sizeof(RAWINPUT);
                    GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param), RID_INPUT, &raw, &pcb_size, sizeof(RAWINPUTHEADER));
                    switch (raw.header.dwType) {
                        case RIM_TYPEMOUSE:
                            // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawmouse
                            break;
                        case RIM_TYPEKEYBOARD:
                            if (GET_RAWINPUT_CODE_WPARAM(w_param) == RIM_INPUT) { // Only handle keyboard with the window is in the foreground
                                auto& keyboard = raw.data.keyboard;
                                USHORT scancode = keyboard.MakeCode |
                                    ((keyboard.Flags & RI_KEY_E1) ? 0xE100 : (keyboard.Flags & RI_KEY_E0) ? 0xE000 : 0); // Distinguish if left or right key is pressed

                                // All Windows has all keyboards convert scancodes to the PS/2 Scan Code table set 1
                                // See Remarks: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawkeyboard#remarks
                                keycode code = ps2_set1_make_scancode_to_keycode(scancode);
#ifdef MYL_DEBUG
                                if (code == key::unknown)
                                    MYTHOS_WARN("Unknown scancode: {}", code);
#endif
								input::process_key(code, (keyboard.Flags & RI_KEY_BREAK) ? input::state::up : input::state::down);
                            }
                            break;
                        case RIM_TYPEHID:
                            // https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawhid
                            break;
                        default:
                            MYTHOS_ERROR("Unknown raw input message, this should not be possible!");
                            break;
                    }

                    break; // Return?
                }
                case WM_CHAR:
                    input::process_typed(static_cast<WCHAR>(w_param));
                    return 0;
            }

        return DefWindowProcA(hwnd, msg, w_param, l_param);
    }
}
#endif

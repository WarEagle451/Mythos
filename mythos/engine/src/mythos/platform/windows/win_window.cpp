#include <mythos/platform/windows/win_window.hpp>
#ifdef MYL_OS_WINDOWS
#   include <mythos/core/application.hpp>
#   include <mythos/core/keyboard.hpp>
#   include <mythos/core/mousecodes.hpp>
#   include <mythos/input.hpp>
#   include <mythos/log.hpp>
#   include <mythos/platform/windows/win_utilities.hpp>

#   include <myl/memory.hpp>

#   include <Windows.h> // WinUser.h
#   include <hidusage.h>

/// MYTODO: Having client_dimensions_to_window_dimensions, AdjustWindowRectEx and GetClientRect/GetWindowRect is dumb, consolidate under 1 function
/// - Maybe split into 2 functions, client to window, window to client

/// MYTODO: Improve RIM_KEYBOARD
/// - https://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/

namespace myth::win {
    MYL_NO_DISCARD static constexpr auto translate_raw_mouse_code(USHORT buttons) noexcept -> mousecode {
        using namespace mouse_button;
        mousecode code{ none };
        if ((buttons & (RI_MOUSE_LEFT_BUTTON_UP | RI_MOUSE_LEFT_BUTTON_DOWN)) != 0)     code |= left;
        if ((buttons & (RI_MOUSE_RIGHT_BUTTON_UP | RI_MOUSE_RIGHT_BUTTON_DOWN)) != 0)   code |= right;
        if ((buttons & (RI_MOUSE_MIDDLE_BUTTON_UP | RI_MOUSE_MIDDLE_BUTTON_DOWN)) != 0) code |= middle;
        if ((buttons & (RI_MOUSE_BUTTON_4_UP | RI_MOUSE_BUTTON_4_DOWN)) != 0)           code |= button4;
        if ((buttons & (RI_MOUSE_BUTTON_5_UP | RI_MOUSE_BUTTON_5_DOWN)) != 0)           code |= button5;

        return code;
    }

    MYL_NO_DISCARD static constexpr auto win_window_style(window_style style) -> DWORD {
        switch (style) {
            using enum window_style;
            case windowed:   return WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_OVERLAPPED | WS_THICKFRAME;
            case fullscreen: return WS_POPUP;
            case popup:      return WS_POPUPWINDOW;
            case unknown:    MYL_FALLTHROUGH;
            default:
                MYTHOS_ERROR("Unknown window style");
                return win_window_style(windowed);
        }
    }

    static auto register_for_raw_input_devices(HWND window_handle) -> void {
        constexpr UINT device_type_count = 4;
        RAWINPUTDEVICE devices[device_type_count]{
            { // Mouse
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_MOUSE,
                /// MYBUG: RIDEV_NOLEGACY prevents the use of the close button
                //.dwFlags = RIDEV_NOLEGACY, // RIDEV_CAPTUREMOUSE; Will mouse buttons will not activate other windows, but how to toggle this?
                .hwndTarget = window_handle
            },
            { // Joystick
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_JOYSTICK,
                .dwFlags = RIDEV_INPUTSINK,
                .hwndTarget = window_handle
            },
            { // Gamepad
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_GAMEPAD,
                .dwFlags = RIDEV_INPUTSINK,
                .hwndTarget = window_handle
            },
            { // Keyboard
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage = HID_USAGE_GENERIC_KEYBOARD,
                /// MYBUG: RIDEV_NOLEGACY blocks WM_CHAR messages, an alternative method of sending typed events must be implemented,
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
                .usUsage     = HID_USAGE_GENERIC_MOUSE,
                .dwFlags     = RIDEV_REMOVE,
                .hwndTarget  = NULL
            },
            { // Joystick
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage     = HID_USAGE_GENERIC_JOYSTICK,
                .dwFlags     = RIDEV_REMOVE,
                .hwndTarget  = NULL
            },
            { // Gamepad
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage     = HID_USAGE_GENERIC_GAMEPAD,
                .dwFlags     = RIDEV_REMOVE,
                .hwndTarget  = NULL
            },
            { // Keyboard
                .usUsagePage = HID_USAGE_PAGE_GENERIC,
                .usUsage     = HID_USAGE_GENERIC_KEYBOARD,
                .dwFlags     = RIDEV_REMOVE,
                .hwndTarget  = NULL
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
        return myl::i32vec2{ // Negative position values are special, the window will be created in the center of the monitor on that axis
            position.x < 0 ? (GetSystemMetrics(SM_CXSCREEN) - dimensions.w) / 2 : position.x,
            position.y < 0 ? (GetSystemMetrics(SM_CYSCREEN) - dimensions.h) / 2 : position.y
        };
    }

    MYL_NO_DISCARD window::window(const window_configuration& config)
        : myth::window(config)
        , m_fullscreen_dimension_cache{ config.dimensions } {
        // Correct for any special values provided
        m_position = correct_negative_position(m_position, m_dimensions);
        m_fullscreen_position_cache = m_position;

        // Setup and register window class
        m_instance = GetModuleHandleA(NULL);

        // Load app window icon
        HICON icon = LoadIcon(m_instance, IDI_APPLICATION);

        constexpr const char* class_name = "mythos_window";
        WNDCLASSA window_class{
            .style = CS_DBLCLKS,
            .lpfnWndProc = process_message,
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = m_instance,
            .hIcon = icon,
            .hCursor = LoadCursor(NULL, IDC_ARROW),
            .hbrBackground = NULL,
            .lpszMenuName = NULL,
            .lpszClassName = class_name
        };

        if (!RegisterClassA(&window_class))
            MYTHOS_FATAL("Window creation failed: {}", win::last_error_as_string(GetLastError()));

        // Adjust window creation position and size based off configuration values
        DWORD window_style = win_window_style(m_style);
        myl::u32 window_ex_style = WS_EX_APPWINDOW;
        int x{}, y{}, w{}, h{};

        if (m_style == window_style::fullscreen) {
            x = 0;
            y = 0;
            w = GetSystemMetrics(SM_CXSCREEN);
            h = GetSystemMetrics(SM_CYSCREEN);

            m_position = { x, y };
            m_dimensions = { w, h };
        }
        else {
            RECT window_rect = { 0, 0, 0, 0 };
            AdjustWindowRectEx(&window_rect, window_style, FALSE, window_ex_style);
            
            x = m_position.x + window_rect.left; // Ignore the border (negative value) for a window, the client area x axis should be the same coordinate on the screen (+ window_rect.left)
            y = m_position.y;                    // Take into account for the toolbar (Windows does this automatically)
            m_position.y -= window_rect.top;     // m_position refers to the client area of the window, adjust it to match when the window is created
    
            // Client size + the size of OS bordering
            w = m_dimensions.w + window_rect.right - window_rect.left;
            h = m_dimensions.h + window_rect.bottom - window_rect.top;
        }

        // Create the window
        m_handle = CreateWindowExA(window_ex_style, class_name, m_title, window_style, x, y, w, h, NULL, NULL, m_instance, NULL);
        if (!m_handle)
            MYTHOS_FATAL("Window creation failed: {}", win::last_error_as_string(GetLastError()));

        m_state = window_state::unknown; // Ensures set_state will not immediately return
        set_state(config.state);

        if (m_state == window_state::maximized) { // Messages will not be posted to the window during creation, simulate WM_MOVE and WM_SIZE
            RECT client{};
            GetClientRect(m_handle, &client);
            m_dimensions = { client.right, client.bottom };

            RECT wind{};
            GetWindowRect(m_handle, &wind);
            m_position = { client.left, wind.bottom - client.bottom + wind.top };
        }

        // Miscellaneous
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

    auto window::set_style(window_style style) -> void {
        if (style == m_style)
            return;

        int x{}, y{}, w{}, h{};
        if (style == window_style::fullscreen) {
            MONITORINFO monitor_info{ .cbSize = sizeof(MONITORINFO) };
            GetMonitorInfoA(MonitorFromWindow(m_handle, MONITOR_DEFAULTTONEAREST), &monitor_info);
            x = static_cast<int>(monitor_info.rcMonitor.left);
            y = static_cast<int>(monitor_info.rcMonitor.top);
            w = static_cast<int>(monitor_info.rcMonitor.right - monitor_info.rcMonitor.left);
            h = static_cast<int>(monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top);

            m_fullscreen_position_cache = m_position;
            m_fullscreen_dimension_cache = m_dimensions;
        }
        else {
            RECT window_rect = { 0, 0, 0, 0 };
            AdjustWindowRectEx(&window_rect, win_window_style(style), FALSE, WS_EX_APPWINDOW);

            if (m_style == window_style::fullscreen){
                x = m_fullscreen_position_cache.x + window_rect.left;
                y = m_fullscreen_position_cache.y + window_rect.top;
                w = m_fullscreen_dimension_cache.w + window_rect.right - window_rect.left;
                h = m_fullscreen_dimension_cache.h + window_rect.bottom - window_rect.top;
            }
            else {
                x = m_position.x + window_rect.left;
                y = m_position.y + window_rect.top;
                w = m_dimensions.w + window_rect.right - window_rect.left;
                h = m_dimensions.h + window_rect.bottom - window_rect.top;
            }
        }

        SetWindowLongPtr(m_handle, GWL_STYLE, win_window_style(style));
        SetWindowPos(m_handle, NULL, x, y, w, h, SWP_FRAMECHANGED);
        ShowWindow(m_handle, SW_SHOW);
        m_style = style;
    }

    auto window::set_state(window_state state) -> void {
        if (state == m_state)
            return;
            
        switch (state) {
            using enum window_state;
            case normal:
                ShowWindow(m_handle, SW_SHOWNORMAL);
                break;
            case maximized:
                ShowWindow(m_handle, SW_SHOWMAXIMIZED);
                break;
            case minimized:
                CloseWindow(m_handle);
                break;
            case unknown:
                MYTHOS_WARN("Unknown window state, maintaining currrent state");
                return;
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

    auto window::close() -> void {
        PostMessageA(m_handle, WM_CLOSE, 0, 0);
    }

    auto window::restore() -> void {
        if (m_style == window_style::fullscreen)
            set_style(window_style::windowed);
        else // WM_SIZE handles window restoring
            ShowWindow(m_handle, SW_RESTORE);
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
    // - WM_HANDHELDFIRST, WM_HANDHELDLAST
    // - WM_DPICHANGED
    // - WM_DEVICECHANGE
    // - WM_APPCOMMAND
    
    // Using WM_INPUT and RIDEV_NOLEGACY disables the following messages;
    // WM_CAPTURECHANGED, WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK, WM_RBUTTONDOWN, WM_RBUTTONUP, WM_RBUTTONDBLCLK, WM_MBUTTONDOWN, WM_MBUTTONUP, WM_MBUTTONDBLCLK, WM_XBUTTONDOWN, WM_XBUTTONUP,
    // WM_XBUTTONDBLCLK, WM_MOUSEACTIVATE, WM_MOUSEHOVER, WM_MOUSEWHEEL, WM_MOUSEHWHEEL, WM_MOUSELEAVE, WM_MOUSEMOVE, WM_NCLBUTTONDOWN, WM_NCLBUTTONUP, WM_NCLBUTTONUP, WM_NCLBUTTONDBLCLK, WM_NCRBUTTONDOWN,
    // WM_NCRBUTTONUP, WM_NCRBUTTONDBLCLK, WM_NCMBUTTONDOWN, WM_NCMBUTTONUP, WM_NCMBUTTONDBLCLK, WM_NCXBUTTONDOWN, WM_NCXBUTTONUP, WM_NCXBUTTONDBLCLK, WM_NCMOUSEMOVE, WM_NCMOUSEHOVER, WM_NCMOUSELEAVE
    // Minic behaviour of them

    // Using WM_INPUT and RIDEV_NOLEGACY also disables the following messages when the docs don't include;
    // WM_CHAR

    auto CALLBACK window::process_message(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) -> LRESULT {
        win::window* target = static_cast<win::window*>(application::get().main_window()); 
        if (target)
            switch (msg) {
                case WM_DESTROY:
                    PostQuitMessage(0);
                    return 0;
                case WM_MOVE: {
                    target->m_position = {
                        static_cast<myl::i32>(static_cast<short>(LOWORD(l_param))),
                        static_cast<myl::i32>(static_cast<short>(HIWORD(l_param)))
                    };
                    event::window_move e(*target, target->m_position);
                    event::fire(e);
                    break;
                }
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
                    return 0;
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
                    UINT size = sizeof(RAWINPUT);
                    myl::buffer raw(size);
                    GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param), RID_HEADER, raw.data(), &size, sizeof(RAWINPUTHEADER));
                    DWORD device_type = raw.as<RAWINPUT>()->header.dwType;

                    GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param), RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
                    raw.reallocate(size);
                    GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param), RID_INPUT, raw.data(), &size, sizeof(RAWINPUTHEADER));
                    auto& data = raw.as<RAWINPUT>()->data;

                    switch (device_type) {
                        case RIM_TYPEMOUSE: {
                            POINT mouse_coords{};
                            GetCursorPos(&mouse_coords);
                            const bool hovered = WindowFromPoint(mouse_coords) == target->m_handle;
                            auto& mouse = data.mouse;
                            
                            // Mouse movement
                            if (mouse.lLastX != 0 || mouse.lLastY != 0) {
                                myl::f32vec2 deltas{ static_cast<myl::f32>(mouse.lLastX), static_cast<myl::f32>(mouse.lLastY) };
                                if (mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
                                    /// MYTODO: Find a way to get RIM_TYPEMOUSE to work with absolute mouse movement
                                    MYTHOS_ERROR("Mythos does not currently support absolute mouse movement on Windows");
                                else
                                    input::process_cursor_delta(deltas);

                                if (hovered) {
                                    ScreenToClient(hwnd, &mouse_coords); // Translate to window coords
                                    input::process_window_cursor_position(myl::f32vec2(static_cast<myl::f32>(mouse_coords.x), static_cast<myl::f32>(mouse_coords.y)));
                                }
                            }

                            // Other mouse inputs
                            const auto& mb_flags = mouse.usButtonFlags;
                            if (mb_flags != 0) {
                                constexpr auto mouse_button_up_flags = RI_MOUSE_LEFT_BUTTON_UP | RI_MOUSE_RIGHT_BUTTON_UP | RI_MOUSE_MIDDLE_BUTTON_UP | RI_MOUSE_BUTTON_4_UP | RI_MOUSE_BUTTON_5_UP;
                                constexpr auto mouse_button_down_flags = RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_MIDDLE_BUTTON_DOWN | RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_5_DOWN;
                                if (hovered) { // Most other mouse inputs should only be evaluated if in the client area of the window
                                    if ((mb_flags & mouse_button_down_flags) != 0)
                                        input::process_mouse_buttons_down(translate_raw_mouse_code(mb_flags));

                                    /// MYTODO: Enable the use of no notch mouse wheels
                                    /// https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawmouse#:~:text=a%20multiple%20of-,WHEEL_DELTA,-%2C%20which%20is%20set

                                    if (mb_flags & RI_MOUSE_WHEEL) { // Vertical mouse wheel
                                        myl::i16 delta_y = static_cast<myl::i16>(mouse.usButtonData);
                                        if (delta_y != 0)
                                            input::process_mouse_wheel({ 0.f, delta_y < 0 ? -1.f : 1.f }); // Flatten to an OS-independent(-1, 1)
                                    }
                                    else if (mb_flags & RI_MOUSE_HWHEEL) { // Horizontal mouse wheel
                                        myl::i16 delta_x = static_cast<myl::i16>(mouse.usButtonData);
                                        if (delta_x != 0)
                                            input::process_mouse_wheel({ delta_x < 0 ? -1.f : 1.f, 0.f }); // Flatten to an OS-independent(-1, 1)
                                    }
                                }
                
                                // Application thinks mouse buttons are pressed, confirm this
                                // Prevents pressing a button in the client area and releasing outside the client area tricking the window into thinking the button is still pressed when it is not
                                if (input::mouse_button_states() != mouse_button::none) // Button(s) are pressed
                                    if ((mb_flags & mouse_button_up_flags) != 0)
                                        input::process_mouse_buttons_up(translate_raw_mouse_code(mb_flags));
                            }
                            return 0;
                        }
                        case RIM_TYPEKEYBOARD:
                            if (GET_RAWINPUT_CODE_WPARAM(w_param) == RIM_INPUT) { // Only handle keyboard with the window is in the foreground
                                auto& keyboard = data.keyboard;
                                USHORT scancode = keyboard.MakeCode |
                                    ((keyboard.Flags & RI_KEY_E1) ? 0xE100 : (keyboard.Flags & RI_KEY_E0) ? 0xE000 : 0); // Distinguish if left or right key is pressed

                                // Windows has all keyboards convert scancodes to the PS/2 Scan Code table set 1
                                // See Remarks: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawkeyboard#remarks
                                keycode code = ps2_set1_make_scancode_to_keycode(scancode);
#ifdef MYL_DEBUG
                                if (code == key::unknown)
                                    MYTHOS_WARN("Unknown scancode: {}", code);
#endif
                                input::process_key(code, (keyboard.Flags & RI_KEY_BREAK) ? input::state::up : input::state::down);
                            }
                            return 0;
                        case RIM_TYPEHID: {
                            RID_DEVICE_INFO rid_info{};
                            UINT rid_info_size = sizeof(RID_DEVICE_INFO);
                            GetRawInputDeviceInfoA(raw.as<RAWINPUT>()->header.hDevice, RIDI_DEVICEINFO, &rid_info, &rid_info_size);

                            input::process_hid(rid_info.hid.dwVendorId, rid_info.hid.dwProductId, data.hid.bRawData, data.hid.dwSizeHid);
                            return 0;
                        }
                        default:
                            MYTHOS_ERROR("Unknown raw input message, this should not be possible!");
                            break;
                    }
                    break;
                }
                case WM_CHAR:
                /// MYTODO: Replace using WM_INPUT
                    input::process_typed(static_cast<WCHAR>(w_param));
                    return 0;
                default:
                    break;
            }

        return DefWindowProcA(hwnd, msg, w_param, l_param);
    }
}
#endif

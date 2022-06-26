#include "win_window.hpp"
#ifdef MYL_PLATFORM_WINDOWS
#	include "win_utils.hpp"

#	include <mythos/core/except.hpp>
#	include <mythos/core/assert.hpp>
#	include <mythos/core/log.hpp>
#	include <mythos/core/keyboard.hpp>
#	include <mythos/core/mouse_codes.hpp>
#	include <mythos/event/app_event.hpp>
#	include <mythos/input.hpp>

#	include <windowsx.h>
#	include <hidusage.h>

/// MYTodo: Should have an option that allows to not use raw input

namespace myl::windows {
	MYL_NO_DISCARD static constexpr mouse_code translate_raw_mouse_code(USHORT buttons) noexcept {
		using namespace mouse_button;
		mouse_code code = none;
		if ((buttons & (RI_MOUSE_LEFT_BUTTON_UP | RI_MOUSE_LEFT_BUTTON_DOWN)) != 0)		code |= left;
		if ((buttons & (RI_MOUSE_RIGHT_BUTTON_UP | RI_MOUSE_RIGHT_BUTTON_DOWN)) != 0)	code |= right;
		if ((buttons & (RI_MOUSE_MIDDLE_BUTTON_UP | RI_MOUSE_MIDDLE_BUTTON_DOWN)) != 0)	code |= middle;
		if ((buttons & (RI_MOUSE_BUTTON_4_UP | RI_MOUSE_BUTTON_4_DOWN)) != 0)			code |= button4;
		if ((buttons & (RI_MOUSE_BUTTON_5_UP | RI_MOUSE_BUTTON_5_DOWN)) != 0)			code |= button5;

		return code;
	}

	static bool cursor_over_client_area(HWND hwnd) {
		RECT client_area{};
		GetClientRect(hwnd, &client_area);

		POINT tl{ .x = client_area.left, .y = client_area.top };
		POINT br{ .x = client_area.right, .y = client_area.bottom };
		ClientToScreen(hwnd, &tl);
		ClientToScreen(hwnd, &br);

		POINT cursor{};
		GetCursorPos(&cursor);

		return cursor.x > tl.x && cursor.x < br.x &&
			cursor.y > tl.y && cursor.y < br.y;
	}

	static key_code get_key_mods() {
		using namespace key_mod;
		key_code mods = none;
		if (GetKeyState(VK_SHIFT) & 0x8000)							mods |= shift;
		if (GetKeyState(VK_CONTROL) & 0x8000)						mods |= control;
		if (GetKeyState(VK_MENU) & 0x8000)							mods |= alt;
		if (GetKeyState(VK_CAPITAL) & 1)							mods |= caps_lock;
		if (GetKeyState(VK_NUMLOCK) & 1)							mods |= num_lock;
		if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)	mods |= super;

		return mods;
	}

	/// MYTodo: Good reference https://github.com/glfw/glfw/blob/master/src/win32_window.c

	LRESULT CALLBACK win32_process_message(HWND hwnd, UINT msg, WPARAM w_param, LPARAM l_param) {
		switch (msg) {
			case WM_CLOSE: {
				event_window_close e{};
				fire_event(e);
			} return 0;
			case WM_SIZE: {
				/// MYTodo: w_param contains SIZE_MAXHIDE, SIZE_MAXIMIZED, SIZE_MAXSHOW, SIZE_MINIMIZED, SIZE_RESTORED
				/// https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-size

				event_window_resize e(LOWORD(l_param), HIWORD(l_param));
				fire_event(e);
			} break;
				/// MYTodo: Eventually get this to be processed by WM_INPUT
			case WM_MOUSEMOVE: // WM_INPUT does bot handle this as Windows already sends this message and it is already relative to the windows position
				input::process_window_cursor_position(f32vec2(static_cast<f32>(GET_X_LPARAM(l_param)), static_cast<f32>(GET_Y_LPARAM(l_param))));
				break;
			case WM_CHAR: /// MYTodo: Eventually get this to be processed by WM_INPUT
				input::process_key_typed(static_cast<WCHAR>(w_param));
				return 0;
			case WM_INPUT: { // https://docs.microsoft.com/en-us/windows/win32/inputdev/raw-input
				// If cursor is over client area
				// - Process window_cursor_position (Handled by WM_MOUSEMOVE)
				// - Process mouse delta
				// - Process_scroll_wheel
				// if app is focused
				// - Process all WM_INPUT except mouse clicks outside of client area
				const bool app_in_forground = GET_RAWINPUT_CODE_WPARAM(w_param) == RIM_INPUT;
				
				RAWINPUT raw{};
				UINT dw_size = sizeof(RAWINPUT);
				GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param), RID_INPUT, &raw, &dw_size, sizeof(RAWINPUTHEADER));
				
				switch (raw.header.dwType) {
					case RIM_TYPEMOUSE: { // Mouse
						/// MYBug: This will only work with deltas, if absolute and 0, 0 = will not fire
						if (raw.data.mouse.lLastX != 0 || raw.data.mouse.lLastY != 0) { // Mouse move
							f32vec2 move{ static_cast<f32>(raw.data.mouse.lLastX), static_cast<f32>(raw.data.mouse.lLastY) };
							MYL_CORE_DEBUG("Mouse delta: [{}, {}]", move.x, move.y);
							raw.data.mouse.usFlags& MOUSE_MOVE_ABSOLUTE ?
								input::process_cursor_delta_given_absolute(move) :
								input::process_cursor_delta(move);
						}

						const bool cursor_over_client_space = cursor_over_client_area(hwnd);
						const auto mb_flags = raw.data.mouse.usButtonFlags;

						if (mb_flags != 0 && cursor_over_client_space) { // Mouse buttons / wheel. (Must be in client space to work)
							if (app_in_forground) { // Mouse buttons
								if ((mb_flags & (RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_MIDDLE_BUTTON_DOWN | RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_5_DOWN)) != 0)
									input::process_mouse_buttons_down(translate_raw_mouse_code(mb_flags));
								if ((mb_flags & (RI_MOUSE_LEFT_BUTTON_UP | RI_MOUSE_RIGHT_BUTTON_UP | RI_MOUSE_MIDDLE_BUTTON_UP | RI_MOUSE_BUTTON_4_UP | RI_MOUSE_BUTTON_5_UP)) != 0)
									input::process_mouse_buttons_up(translate_raw_mouse_code(mb_flags));
							}

							if (mb_flags & RI_MOUSE_WHEEL) { // Mouse wheel (vertical)
								i16 y_delta = static_cast<i16>(raw.data.mouse.usButtonData);
								if (y_delta != 0) // Flatten the input to an OS-independent(-1, 1)
									y_delta = y_delta < 0 ? -1 : 1;
								input::process_mouse_wheel({ 0.f, static_cast<f32>(y_delta) });
							}

							if (mb_flags & RI_MOUSE_HWHEEL) { // Mouse wheel (horizontal) 
								i16 x_delta = static_cast<i16>(raw.data.mouse.usButtonData);
								if (x_delta != 0) // Flatten the input to an OS-independent(-1, 1)
									x_delta = x_delta < 0 ? -1 : 1;
								input::process_mouse_wheel({ static_cast<f32>(x_delta), 0.f });
							}
						}
					} break;
					case RIM_TYPEKEYBOARD: // Keyboard
						if (app_in_forground) {
							auto& keyboard = raw.data.keyboard;
							USHORT scancode = keyboard.MakeCode | ((keyboard.Flags & RI_KEY_E1) ? 0xE100 : (keyboard.Flags & RI_KEY_E0) ? 0xE000 : 0);
							input::state state = (keyboard.Flags & RI_KEY_BREAK) ? input::state::up : input::state::down;
							key_code key = translate_ps2_set_1_make(scancode); // PS/2 set 1 makecodes are always used in Windows
							input::process_key(key, state);
							/// MYTodo: Should do what WM_CHAR is doing here
						} break;
					case RIM_TYPEHID: // Not a keyboard or mouse
						MYL_CORE_WARN("Unknown device sent WM_INPUT");
						/// MYTodo: Non-keyboard, non-mouse input
						break;
					default: break;
				}
			} return 0;
			///case WM_INPUT_DEVICE_CHANGE:
			///	break; /// MYTodo
			///case WM_DPICHANGED: /// MYTodo: WM_DPICHANGED
			///	break; /// https://docs.microsoft.com/en-us/windows/win32/hidpi/wm-dpichanged
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case WM_ERASEBKGND: // Notify the OS that erasing will be handled by the app to prevent flicker
				return 1;
		}
	
		return DefWindowProcA(hwnd, msg, w_param, l_param);
	}

	static constexpr DWORD get_style(window_style style) {
		DWORD win_style = 0;
		switch (style) {
			using enum window_style;
			case fullscreen: win_style |= WS_POPUP; break;
			case maximized: win_style |= WS_MAXIMIZE; MYL_FALLTHROUGH;
			case windowed: win_style |=
				WS_SYSMENU | WS_CAPTION |
				WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
				WS_OVERLAPPED | WS_THICKFRAME;
				break;
		}
		return win_style;
	}

	static i32vec2 get_window_position(HWND hwnd) {
		RECT win_rect{};
		GetWindowRect(hwnd, &win_rect);
		return { static_cast<i32>(win_rect.left), static_cast<i32>(win_rect.top) };
	}

	window::window(const config& a_config)
		: myl::window()
		, m_size(a_config.size) {
		m_instance = GetModuleHandleA(0);
		
		constexpr const char* window_class_name = "mythos_window_class";

		// Setup and register window class
		HICON icon = LoadIcon(m_instance, IDI_APPLICATION);
		WNDCLASSA wc{
			.style = CS_DBLCLKS,
			.lpfnWndProc = win32_process_message,
			.cbClsExtra = 0,
			.cbWndExtra = 0,
			.hInstance = m_instance,
			.hIcon = icon,
			.hCursor = LoadCursor(NULL, IDC_ARROW),
			.hbrBackground = NULL,
			.lpszClassName = window_class_name,
		};

		if (!RegisterClassA(&wc))
			throw core_runtime_error("Windows window registration failed");

		// Create window
		DWORD window_style = get_style(a_config.style);
		u32 window_ex_style = WS_EX_APPWINDOW;

		// Obtain size of the border
		RECT border_rect = { 0, 0, 0, 0 };
		AdjustWindowRectEx(&border_rect, window_style, FALSE, window_ex_style);
		
		// Client area position - border rect. (In this case, the border rect is negative)
		int window_x = (a_config.style == window_style::fullscreen) ? 0 : a_config.postion.x - border_rect.left;
		int window_y = (a_config.style == window_style::fullscreen) ? 0 : a_config.postion.y - border_rect.top;

		// Client size + the size of OS bordering
		int window_w = (a_config.style == window_style::fullscreen) ? GetSystemMetrics(SM_CXSCREEN) : a_config.size.w + border_rect.right - border_rect.left;
		int window_h = (a_config.style == window_style::fullscreen) ? GetSystemMetrics(SM_CYSCREEN) : a_config.size.h + border_rect.bottom - border_rect.top;

		m_handle = CreateWindowExA(window_ex_style, window_class_name, a_config.name.c_str(), window_style, window_x, window_y, window_w, window_h, nullptr, nullptr, m_instance, nullptr);
		if (!m_handle)
			throw core_runtime_error("Windows window creation failed");

		if (a_config.style == window_style::maximized || a_config.style == window_style::fullscreen) {
			RECT win_rect{};
			GetWindowRect(m_handle, &win_rect);
			m_size = {
				static_cast<u32>(win_rect.right - win_rect.left - (border_rect.right - border_rect.left)),
				static_cast<u32>(win_rect.bottom - win_rect.top - (border_rect.bottom - border_rect.top))
			};
		}

		// Show the window
		bool should_activate = true;
		i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
		// If initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
		// If initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE;
		ShowWindow(m_handle, show_window_command_flags);

		/// MYTodo: figure out how to register multiple devices, also check if above are present first

		std::vector<RAWINPUTDEVICE> rids;
		rids.push_back(RAWINPUTDEVICE{ // Mouse
				.usUsagePage = HID_USAGE_PAGE_GENERIC,
				.usUsage = HID_USAGE_GENERIC_MOUSE,
				.dwFlags = RIDEV_EXINPUTSINK,
				.hwndTarget = m_handle
			});

		rids.push_back(RAWINPUTDEVICE{ // Keyboard
				.usUsagePage = HID_USAGE_PAGE_GENERIC,
				.usUsage = HID_USAGE_GENERIC_KEYBOARD,
				.dwFlags = RIDEV_EXINPUTSINK, // RIDEV_NOLEGACY: No WM_KEYDOWN, WM_SYSKEYDOWN, etc will occur
				.hwndTarget = m_handle
			});

		if (!RegisterRawInputDevices(rids.data(), static_cast<UINT>(rids.size()), sizeof(RAWINPUTDEVICE)))
			throw core_runtime_error("Windows - Failed to register raw input devices");
		/// MYTodo: To receive WM_INPUT_DEVICE_CHANGE messages, an application must specify the \
		/// RIDEV_DEVNOTIFY flag for each device class that is specified by the usUsagePageand usUsage \
		/// fields of the RAWINPUTDEVICE structure.By default, an application does not receive WM_INPUT_DEVICE_CHANGE \
		/// notifications for raw input device arrivals and removal. \
		/// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerrawinputdevices
	}

	window::~window() {
		if (m_handle) {
			DestroyWindow(m_handle);
			m_handle = nullptr;
		}
	}

	void window::update() {
		MSG message;
		while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessageA(&message);
		}
	}

	void window::on_resize(const u32vec2& a_size) {
		m_size = a_size;
	}

	void window::toggle_fullscreen() {
		LONG_PTR style = GetWindowLongPtr(m_handle, GWL_STYLE);
		if (style & WS_OVERLAPPEDWINDOW) { // To fullscreen
			MONITORINFO monitor_info{};
			monitor_info.cbSize = sizeof(monitor_info);
			GetMonitorInfo(MonitorFromWindow(m_handle, MONITOR_DEFAULTTONEAREST), &monitor_info);
			
			SetWindowLongPtr(m_handle, GWL_STYLE, get_style(window_style::fullscreen)); /// MYBug: When this changes it flashes to old windows border, make instant

			m_cached_size = m_size; // Fullscreen to windowed needs to know previous window size
			m_position_pre_fullscreen = get_window_position(m_handle); // Fullscreen to windowed needs to know previous position
			LONG w = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
			LONG h = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
			SetWindowPos(m_handle, nullptr,
				monitor_info.rcMonitor.left,
				monitor_info.rcMonitor.top,
				w,
				h,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
		}
		else { // To windowed or maximized
			window_style new_win_style = /// MYBug: Fullscreen to maximized not being detected
				(GetSystemMetrics(SM_CXSCREEN) == static_cast<int>(m_cached_size.x) &&
				GetSystemMetrics(SM_CYSCREEN) == static_cast<int>(m_cached_size.y)) ? window_style::maximized : window_style::windowed;
			SetWindowLongPtr(m_handle, GWL_STYLE, get_style(new_win_style));

			SetWindowPos(m_handle,  nullptr,
				 m_position_pre_fullscreen.x, m_position_pre_fullscreen.y,
				 m_cached_size.w, m_cached_size.h,
				 SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
			);
		}

		ShowWindow(m_handle, SW_SHOW);
	}
}
#endif
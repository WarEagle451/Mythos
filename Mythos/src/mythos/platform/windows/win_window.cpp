#include "win_window.hpp"
#ifdef MYL_PLATFORM_WINDOWS
#	include "win_utils.hpp"

#	include <mythos/core/except.hpp>
#	include <mythos/core/assert.hpp>
#	include <mythos/core/log.hpp>
#	include <mythos/core/key_codes.hpp>
#	include <mythos/core/mouse_codes.hpp>
#	include <mythos/event/app_event.hpp>
#	include <mythos/input.hpp>

#	include <windowsx.h>
#	include <hidusage.h>

// https://docs.microsoft.com/en-us/windows/win32/inputdev/raw-input

/// MYTodo: Figure out event_key_typed, probs WM_CHAR

namespace myl::windows {
	MYL_NO_DISCARD constexpr mouse_code translate_raw_mouse_code(USHORT buttons) {
		using namespace mouse_button;
		mouse_code code = none;
		if ((buttons & (RI_MOUSE_LEFT_BUTTON_UP | RI_MOUSE_LEFT_BUTTON_DOWN)) != 0)		code |= left;
		if ((buttons & (RI_MOUSE_RIGHT_BUTTON_UP | RI_MOUSE_RIGHT_BUTTON_DOWN)) != 0)	code |= right;
		if ((buttons & (RI_MOUSE_MIDDLE_BUTTON_UP | RI_MOUSE_MIDDLE_BUTTON_DOWN)) != 0)	code |= middle;
		if ((buttons & (RI_MOUSE_BUTTON_4_UP | RI_MOUSE_BUTTON_4_DOWN)) != 0)			code |= button4;
		if ((buttons & (RI_MOUSE_BUTTON_5_UP | RI_MOUSE_BUTTON_5_DOWN)) != 0)			code |= button5;

		return code;
	}

	MYL_NO_DISCARD static constexpr key_code translate_raw_scancode(USHORT scancode) {
		switch (scancode) { // From: https://source.chromium.org/chromium/chromium/src/+/main:ui/events/keycodes/dom/dom_code_data.inc
			using namespace key;
			case 0x0001: return escape;
			case 0x0002: return n1;
			case 0x0003: return n2;
			case 0x0004: return n3;
			case 0x0005: return n4;
			case 0x0006: return n5;
			case 0x0007: return n6;
			case 0x0008: return n7;
			case 0x0009: return n8;
			case 0x000A: return n9;
			case 0x000B: return n0;
			case 0x000C: return dash;
			case 0x000D: return equal;
			case 0x000E: return backspace;
			case 0x000F: return tab;
			case 0x0010: return q;
			case 0x0011: return w;
			case 0x0012: return e;
			case 0x0013: return r;
			case 0x0014: return t;
			case 0x0015: return y;
			case 0x0016: return u;
			case 0x0017: return i;
			case 0x0018: return o;
			case 0x0019: return p;
			case 0x001A: return left_bracket;
			case 0x001B: return right_bracket;
			case 0x001C: return enter;
			case 0x001D: return left_control;
			case 0x001E: return a;
			case 0x001F: return s;
			case 0x0020: return d;
			case 0x0021: return f;
			case 0x0022: return g;
			case 0x0023: return h;
			case 0x0024: return j;
			case 0x0025: return k;
			case 0x0026: return l;
			case 0x0027: return semicolon;
			case 0x0028: return apostrophe;
			case 0x0029: return grave_accent;
			case 0x002A: return left_shift;
			case 0x002B: return backslash;
			case 0x002C: return z;
			case 0x002D: return x;
			case 0x002E: return c;
			case 0x002F: return v;
			case 0x0030: return b;
			case 0x0031: return n;
			case 0x0032: return m;
			case 0x0033: return comma;
			case 0x0034: return period;
			case 0x0035: return slash;
			case 0x0036: return right_shift;
			case 0x0037: return multiply;
			case 0x0038: return left_alt;
			case 0x0039: return space;
			case 0x003A: return caps_lock;
			case 0x003B: return f1;
			case 0x003C: return f2;
			case 0x003D: return f3;
			case 0x003E: return f4;
			case 0x003F: return f5;
			case 0x0040: return f6;
			case 0x0041: return f7;
			case 0x0042: return f8;
			case 0x0043: return f9;
			case 0x0044: return f10;
			case 0x0045: return pause;
			case 0x0046: return scroll_lock;
			case 0x0047: return kp7;
			case 0x0048: return kp8;
			case 0x0049: return kp9;
			case 0x004A: return subtract;
			case 0x004B: return kp4;
			case 0x004C: return kp5;
			case 0x004D: return kp6;
			case 0x004E: return add;
			case 0x004F: return kp1;
			case 0x0050: return kp2;
			case 0x0051: return kp3;
			case 0x0052: return kp0;
			case 0x0053: return decimal;
			case 0x0054: return print_screen; // alt + print screen. Force it to register as printscreen

			case 0x0057: return f11;
			case 0x0058: return f12;

			case 0x0064: return f13;
			case 0x0065: return f14;
			case 0x0066: return f15;
			case 0x0067: return f16;
			case 0x0068: return f17;
			case 0x0069: return f18;
			case 0x006A: return f19;
			case 0x006B: return f20;
			case 0x006C: return f21;
			case 0x006D: return f22;
			case 0x006E: return f23;

			case 0x0076: return f24;

			case 0xE01C: return kp_enter;
			case 0xE01D: return right_control;
			case 0xE035: return divide;
			case 0xE037: return print_screen;
			case 0xE038: return right_alt;
			case 0xE045: return num_lock;
			case 0xE047: return home;
			case 0xE048: return up;
			case 0xE049: return page_up;
			case 0xE04B: return left;
			case 0xE04D: return right; 
			case 0xE04F: return end;
			case 0xE050: return down;
			case 0xE051: return page_down;
			case 0xE052: return insert;
			case 0xE053: return delete_key;
			case 0xE05B: return left_super;
			case 0xE05C: return right_super;

			case 0x0000: MYL_FALLTHROUGH;
			case 0x0070: MYL_FALLTHROUGH;
			case 0x0071: MYL_FALLTHROUGH;
			case 0x0072: MYL_FALLTHROUGH;
			case 0x0073: MYL_FALLTHROUGH;
			case 0x0077: MYL_FALLTHROUGH;
			case 0x0078: MYL_FALLTHROUGH;
			case 0x0079: MYL_FALLTHROUGH;
			case 0x007B: MYL_FALLTHROUGH;
			case 0x007D: MYL_FALLTHROUGH;
			case 0x007E: MYL_FALLTHROUGH;
			case 0xE008: MYL_FALLTHROUGH;
			case 0xE00A: MYL_FALLTHROUGH;
			case 0xE010: MYL_FALLTHROUGH;
			case 0xE017: MYL_FALLTHROUGH;
			case 0xE018: MYL_FALLTHROUGH;
			case 0xE019: MYL_FALLTHROUGH;
			case 0xE020: MYL_FALLTHROUGH;
			case 0xE021: MYL_FALLTHROUGH;
			case 0xE022: MYL_FALLTHROUGH;
			case 0xE024: MYL_FALLTHROUGH;
			case 0xE02C: MYL_FALLTHROUGH;
			case 0xE02E: MYL_FALLTHROUGH;
			case 0xE030: MYL_FALLTHROUGH;
			case 0xE032: MYL_FALLTHROUGH;
			case 0xE03B: MYL_FALLTHROUGH;
			case 0x0056: MYL_FALLTHROUGH;
			case 0x0059: MYL_FALLTHROUGH;
			case 0xE05D: MYL_FALLTHROUGH;
			case 0xE05E: MYL_FALLTHROUGH;
			case 0xE065: MYL_FALLTHROUGH;
			case 0xE066: MYL_FALLTHROUGH;
			case 0xE067: MYL_FALLTHROUGH;
			case 0xE068: MYL_FALLTHROUGH;
			case 0xE069: MYL_FALLTHROUGH;
			case 0xE06A: MYL_FALLTHROUGH;
			case 0xE06B: MYL_FALLTHROUGH;
			case 0xE06C: MYL_FALLTHROUGH;
			case 0xE06D: MYL_CORE_DEBUG("Valid unhandled key pressed: {}", scancode); MYL_FALLTHROUGH;
			default: return unknown;
		}
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

	LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
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
			case WM_INPUT: {
				// If cursor is over client area
				// - Process window_cursor_position (Handled by WM_MOUSEMOVE)
				// - Process mouse delta
				// - Process_scroll_wheel
				// if app is focused
				// - Process all WM_INPUT except mouse clicks outside of client area
				
				const bool cursor_over_client_space = cursor_over_client_area(hwnd);
				const bool app_in_forground = GET_RAWINPUT_CODE_WPARAM(w_param) == RIM_INPUT;

				RAWINPUT raw{};
				UINT dw_size = sizeof(RAWINPUT);
				GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param), RID_INPUT, &raw, &dw_size, sizeof(RAWINPUTHEADER));

				switch (raw.header.dwType) {
					case RIM_TYPEMOUSE: // Mouse
						if (auto mb_flags = raw.data.mouse.usButtonFlags; mb_flags != 0 && cursor_over_client_space) { // Mouse buttons / wheel. (Must be in client space to work)
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

						/// MYBug: This will only work with deltas, if absolute and 0, 0 = will not fire
						if (raw.data.mouse.lLastX != 0 || raw.data.mouse.lLastY != 0) { // Mouse move
							f32vec2 move{ static_cast<f32>(raw.data.mouse.lLastX), static_cast<f32>(raw.data.mouse.lLastY) };
							raw.data.mouse.usFlags& MOUSE_MOVE_ABSOLUTE ?
								input::process_cursor_delta_given_absolute(move) : /// MYTodo: How is this supposed to be handled?
								input::process_cursor_delta(move);
						} break;
					case RIM_TYPEKEYBOARD: // Keyboard
						if (app_in_forground) {
							/// MYBug: Num lock is being read as pause
							/// MYBug: When num lock is disengaged, key is still sending key_numbers not keys like home, end, etc
							
							auto& keyboard = raw.data.keyboard;
							USHORT scancode = keyboard.MakeCode | ((keyboard.Flags & RI_KEY_E1) ? 0xE100 : (keyboard.Flags & RI_KEY_E0) ? 0xE000 : 0);
							input::process_key(
								translate_raw_scancode(scancode),
								(keyboard.Flags & RI_KEY_BREAK) ? input::state::up : input::state::down,
								0); /// MYTodo: Get key repeats
						} break;
					case RIM_TYPEHID: // Not a keyboard or mouse
						MYL_CORE_WARN("Unknown device sent WM_INPUT");
						break;
					default:
						/// MYTodo: DefRawInputProc();
						break;
				}
			} break;
			///case WM_CHAR: /// MYTodo: WM_CHAR: https://docs.microsoft.com/en-us/windows/win32/learnwin32/keyboard-input
			///	break; /// https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-char
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
		i32 client_x = a_config.postion.x;
		i32 client_y = a_config.postion.y;
		u32 client_width = a_config.size.w;
		u32 client_height = a_config.size.h;

		i32 window_x = client_x;
		i32 window_y = client_y;
		u32 window_width = client_width;
		u32 window_height = client_height;

		u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
		u32 window_ex_style = WS_EX_APPWINDOW;

		///window_style |= WS_MAXIMIZE;
		window_style |= WS_MINIMIZEBOX;
		window_style |= WS_MAXIMIZEBOX;
		window_style |= WS_THICKFRAME;

		// Obtain size of the border
		RECT border_rect = { 0, 0, 0, 0 };
		AdjustWindowRectEx(&border_rect, window_style, FALSE, window_ex_style);
		
		// In this case, the border rect is negative
		window_x -= border_rect.left;
		window_y -= border_rect.top;

		// Grow by the size of the OS bordering
		window_width += border_rect.right - border_rect.left;
		window_height += border_rect.bottom - border_rect.top;

		m_handle = CreateWindowExA(window_ex_style, window_class_name, a_config.name.c_str(), window_style, window_x, window_y, window_width, window_height, 0, 0, m_instance, 0);
		if (!m_handle)
			throw core_runtime_error("Windows window creation failed");

		if (window_style & WS_MAXIMIZE) { /// MYTodo: There has to be a better way to detect if the screen is maximized
			RECT win_size{};
			GetWindowRect(m_handle, &win_size);
			m_size = {
				static_cast<u32>(win_size.right - win_size.left - (border_rect.right - border_rect.left)),
				static_cast<u32>(win_size.bottom - win_size.top - (border_rect.bottom - border_rect.top))
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
				.usUsage = HID_USAGE_GENERIC_KEYBOARD, /// MYTodo: Below comment for mouse
				.dwFlags = RIDEV_EXINPUTSINK | RIDEV_NOLEGACY, // RIDEV_NOLEGACY: No WM_KEYDOWN, WM_SYSKEYDOWN, etc will occur
				.hwndTarget = m_handle
			});

		if (!RegisterRawInputDevices(rids.data(), rids.size(), sizeof(RAWINPUTDEVICE)))
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
}
#endif
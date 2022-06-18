#include "win_window.hpp"
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/except.hpp>
#	include <mythos/core/assert.hpp>
#	include <mythos/core/log.hpp>
#	include <mythos/core/key_codes.hpp>
#	include <mythos/core/mouse_codes.hpp>
#	include <mythos/event/app_event.hpp>

#	include <mythos/input.hpp>

#	include <windowsx.h>
#	include <hidusage.h>
#	include <Xinput.h>

/// MYTodo: Figure out event_key_typed

#	define MYL_WM_INPUT 1

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

	MYL_NO_DISCARD static constexpr key_code translate_key_code(WPARAM w_param, LPARAM l_param) { /// MYTodo: remove
		switch (static_cast<u16>(w_param)) { // Key code it a u16 from the w_param
			using namespace key;
			case VK_TAB: return tab;
			case VK_RETURN: return enter;
			case VK_ESCAPE: return escape;
			case VK_SPACE: return space;
			case VK_OEM_7: return apostrophe;
			case VK_OEM_COMMA: return comma;
			case VK_OEM_MINUS: return dash;
			case VK_OEM_PERIOD: return period;
			case VK_OEM_2: return slash;
			case 0x30: return n0;
			case 0x31: return n1;
			case 0x32: return n2;
			case 0x33: return n3;
			case 0x34: return n4;
			case 0x35: return n5;
			case 0x36: return n6;
			case 0x37: return n7;
			case 0x38: return n8;
			case 0x39: return n9;
			case VK_OEM_1: return semicolon;
			case VK_OEM_PLUS: return equal;
			case 0x41: return a;
			case 0x42: return b;
			case 0x43: return c;
			case 0x44: return d;
			case 0x45: return e;
			case 0x46: return f;
			case 0x47: return g;
			case 0x48: return h;
			case 0x49: return i;
			case 0x4a: return j;
			case 0x4b: return k;
			case 0x4c: return l;
			case 0x4d: return m;
			case 0x4e: return n;
			case 0x4f: return o;
			case 0x50: return p;
			case 0x51: return q;
			case 0x52: return r;
			case 0x53: return s;
			case 0x54: return t;
			case 0x55: return u;
			case 0x56: return v;
			case 0x57: return w;
			case 0x58: return x;
			case 0x59: return y;
			case 0x5a: return z;
			case VK_OEM_3: return grave_accent;
			case VK_OEM_4: return left_bracket;
			case VK_OEM_5: return backslash;
			case VK_OEM_6: return right_bracket;
			case VK_BACK: return backspace;
			case VK_INSERT: return insert;
			case VK_DELETE: return delete_key;
			case VK_RIGHT: return right;
			case VK_LEFT: return left;
			case VK_DOWN: return down;
			case VK_UP: return up;
			case VK_PRIOR: return page_up;
			case VK_NEXT: return page_down;
			case VK_HOME: return home;
			case VK_END: return end;
			case VK_CAPITAL: return caps_lock;
			case VK_SCROLL: return scroll_lock;
			case VK_NUMLOCK: return num_lock;
			case VK_SNAPSHOT: return print_screen; 	/// MYBug: Print screen does not work
			case VK_PAUSE: return pause;
			case VK_F1: return f1;
			case VK_F2: return f2;
			case VK_F3: return f3;
			case VK_F4: return f4;
			case VK_F5: return f5;
			case VK_F6: return f6;
			case VK_F7: return f7;
			case VK_F8: return f8;
			case VK_F9: return f9;
			case VK_F10: return f10;
			case VK_F11: return f11;
			case VK_F12: return f12;
			case VK_F13: return f13;
			case VK_F14: return f14;
			case VK_F15: return f15;
			case VK_F16: return f16;
			case VK_F17: return f17;
			case VK_F18: return f18;
			case VK_F19: return f19;
			case VK_F20: return f20;
			case VK_F21: return f21;
			case VK_F22: return f22;
			case VK_F23: return f23;
			case VK_F24: return f24;
			case VK_NUMPAD0: return kp0;
			case VK_NUMPAD1: return kp1;
			case VK_NUMPAD2: return kp2;
			case VK_NUMPAD3: return kp3;
			case VK_NUMPAD4: return kp4;
			case VK_NUMPAD5: return kp5;
			case VK_NUMPAD6: return kp6;
			case VK_NUMPAD7: return kp7;
			case VK_NUMPAD8: return kp8;
			case VK_NUMPAD9: return kp9;
			case VK_DECIMAL: return decimal;
			case VK_DIVIDE: return divide;
			case VK_MULTIPLY: return multiply;
			case VK_SUBTRACT: return subtract;
			case VK_ADD: return add;
			case VK_LWIN: return left_super;
			case VK_RWIN: return right_super;

			// Special handling

			case VK_MENU: {
				bool extended = (l_param & 0x01000000) == 0;
				return extended ? left_alt : right_alt;
			}
			case VK_CONTROL: {
				bool extended = (l_param & 0x01000000) == 0;
				return extended ? left_control : right_control;
			}
			case VK_SHIFT: {
				UINT scancode = (l_param & 0x00ff0000) >> 16;
				return MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX) == VK_LSHIFT ? left_shift : right_shift;
			}

			default: return unknown;
		}
	}

/// https://github.com/o3de/o3de/blob/development/Code/Framework/AzFramework/Platform/Windows/AzFramework/Windowing/NativeWindow_Windows.cpp
/// MYTodo: Above is a good ref for windows messages

	static bool cursor_over_client_area(HWND hwnd) {
		RECT client_area{};
		GetClientRect(hwnd, &client_area);

		POINT tl{ .x = client_area.left, .y = client_area.top };
		POINT br{ .x = client_area.right, .y = client_area.bottom };
		ClientToScreen(hwnd, &tl);
		ClientToScreen(hwnd, &br);

		POINT cursor{};
		GetCursorPos(&cursor);

		return
			cursor.x > tl.x && cursor.x < br.x &&
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
				/// MYTodo: Eventually get this to ve processed by WM_INPUT
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

				UINT dw_size = sizeof(RAWINPUT);
				static BYTE lpb[sizeof(RAWINPUT)]{};
				GetRawInputData(reinterpret_cast<HRAWINPUT>(l_param), RID_INPUT, lpb, &dw_size, sizeof(RAWINPUTHEADER));
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);

				switch (raw->header.dwType) {
					case RIM_TYPEMOUSE: // Mouse
						if (auto mb_flags = raw->data.mouse.usButtonFlags; mb_flags != 0 && cursor_over_client_space) { // Mouse buttons / wheel. (Must be in client space to work)
							if (app_in_forground) { // Mouse buttons
								if ((mb_flags & (RI_MOUSE_LEFT_BUTTON_DOWN | RI_MOUSE_RIGHT_BUTTON_DOWN | RI_MOUSE_MIDDLE_BUTTON_DOWN | RI_MOUSE_BUTTON_4_DOWN | RI_MOUSE_BUTTON_5_DOWN)) != 0)
									input::process_mouse_buttons_down(translate_raw_mouse_code(mb_flags));
								if ((mb_flags & (RI_MOUSE_LEFT_BUTTON_UP | RI_MOUSE_RIGHT_BUTTON_UP | RI_MOUSE_MIDDLE_BUTTON_UP | RI_MOUSE_BUTTON_4_UP | RI_MOUSE_BUTTON_5_UP)) != 0)
									input::process_mouse_buttons_up(translate_raw_mouse_code(mb_flags));
							}

							if (mb_flags & RI_MOUSE_WHEEL) { // Mouse wheel (vertical)
								i16 y_delta = static_cast<i16>(raw->data.mouse.usButtonData);
								if (y_delta != 0) // Flatten the input to an OS-independent(-1, 1)
									y_delta = y_delta < 0 ? -1 : 1;
								input::process_mouse_wheel({ 0.f, static_cast<f32>(y_delta) });
							}

							if (mb_flags & RI_MOUSE_HWHEEL) { // Mouse wheel (horizontal) 
								i16 x_delta = static_cast<i16>(raw->data.mouse.usButtonData);
								if (x_delta != 0) // Flatten the input to an OS-independent(-1, 1)
									x_delta = x_delta < 0 ? -1 : 1;
								input::process_mouse_wheel({ static_cast<f32>(x_delta), 0.f });
							}
						}

						/// MYBug: This will only work with deltas, if absolute and 0, 0 = will not fire
						if (raw->data.mouse.lLastX != 0 || raw->data.mouse.lLastY != 0) { // Mouse move
							f32vec2 move{ static_cast<f32>(raw->data.mouse.lLastX), static_cast<f32>(raw->data.mouse.lLastY) };
							raw->data.mouse.usFlags& MOUSE_MOVE_ABSOLUTE ?
								input::process_cursor_delta_given_absolute(move) : /// MYTodo: How is this supposed to be handled?
								input::process_cursor_delta(move);
						} break;
					case RIM_TYPEKEYBOARD: // Keyboard
						if (app_in_forground) {
							auto& keyboard = raw->data.keyboard;

							if (keyboard.Flags & RI_KEY_E0)
								; /// MYTodo: 
							if (keyboard.Flags & RI_KEY_E1)
								; /// MYTodo: 
							if (keyboard.Flags & RI_KEY_TERMSRV_SET_LED)
								; /// MYTodo: 
							if (keyboard.Flags & RI_KEY_TERMSRV_SHADOW)
								; /// MYTodo: 
							keyboard.ExtraInformation; /// MYTodo:

							/// MYTodo: probs need to register keyboard (also multiple mice and keyboards)
							/// MYTodo: Confirm that 0 or 1 has to be in the first bit
							input::state key_state = (keyboard.Flags & RI_KEY_MAKE) ? input::state::down : input::state::up;
							u32 key_repeats = 0;
							input::process_key(translate_key_code(keyboard.VKey, l_param), key_state, key_repeats);
							///input::process_key(translate_raw_scancode(keyboard.MakeCode), key_state, key_repeats);
						} break;
					case RIM_TYPEHID: // Not a keyboard or mouse
						MYL_CORE_WARN("Unknown device sent WM_INPUT");
						break;
					default: MYL_CORE_ASSERT(false, "WM_INPUT value not valid"); break;
				}
			} break;
			case WM_CHAR: /// MYTodo: WM_CHAR: https://docs.microsoft.com/en-us/windows/win32/learnwin32/keyboard-input
				break; /// https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-char
			case WM_ACTIVATE: /// MYTodo: WM_ACTIVATE
				break; /// https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-activate
			case WM_INPUT_DEVICE_CHANGE:
				break; /// MYTodo
#if !MYL_WM_INPUT
			case WM_KEYDOWN: MYL_FALLTHROUGH; /// MYTodo: Lumber yard doesn't use this? why?
			case WM_SYSKEYDOWN:
				input::process_key(translate_key_code(w_param, l_param), input::state::down, static_cast<u32>(LOWORD(l_param))); // First 16 bits contain the repeat count
				break;
			case WM_KEYUP: MYL_FALLTHROUGH; /// MYTodo: Lumber yard doesn't use this? why?
			case WM_SYSKEYUP:
				input::process_key(translate_key_code(w_param, l_param), input::state::up, 0);
				break;
#endif
			case WM_DPICHANGED: /// MYTodo: WM_DPICHANGED
				break; /// https://docs.microsoft.com/en-us/windows/win32/hidpi/wm-dpichanged
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case WM_ERASEBKGND: // Notify the OS that erasing will be handled by the app to prevent flicker
				return 1;
		}
	
		return DefWindowProcA(hwnd, msg, w_param, l_param);
	}

	std::string GetLastError_as_string() {
		DWORD error_id = GetLastError();
		if (error_id == 0)
			return std::string(); // No error

		LPSTR message_buf = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									 nullptr, error_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&message_buf), 0, nullptr);

		std::string message(message_buf, size);
		LocalFree(message_buf);
		return message;
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
#if 1
		RAWINPUTDEVICE rid{ /// MYTodo: Should only do if exists
			.usUsagePage = HID_USAGE_PAGE_GENERIC,
			.usUsage = HID_USAGE_GENERIC_MOUSE,
			.dwFlags = RIDEV_EXINPUTSINK,
			.hwndTarget = m_handle
		};

		/// MYTodo: figure out how to register both keyboards

		if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
			throw core_runtime_error("Windows - Failed to register raw input devices");
		/// MYTodo: To receive WM_INPUT_DEVICE_CHANGE messages, an application must specify the
		/// RIDEV_DEVNOTIFY flag for each device class that is specified by the usUsagePageand usUsage
		/// fields of the RAWINPUTDEVICE structure.By default, an application does not receive WM_INPUT_DEVICE_CHANGE
		/// notifications for raw input device arrivaland removal.
		/// https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerrawinputdevices
#else
		/// I Don't understand why this fails
		UINT rids_count = 0;
		UINT list_size = sizeof(RAWINPUTDEVICELIST);
		GetRawInputDeviceList(nullptr, &rids_count, list_size);
		if (rids_count) {
			std::vector<RAWINPUTDEVICELIST> rid_list(rids_count);
			rids_count = GetRawInputDeviceList(rid_list.data(), &rids_count, sizeof(RAWINPUTDEVICELIST));
			std::vector<RAWINPUTDEVICE> rids{};
			for (auto& device : rid_list) {
				UINT cb_size = sizeof(RID_DEVICE_INFO); /// MAKE SURE THIS IS CORRECT, DO YOU DONT HAVE TO CHECK THE SIZE
				///GetRawInputDeviceInfoA(device.hDevice, RIDI_DEVICEINFO, nullptr, &cb_size);
				RID_DEVICE_INFO info{};
				GetRawInputDeviceInfoA(device.hDevice, RIDI_DEVICEINFO, &info, &cb_size);

				if (raw_input_device_supported(info))
					rids.push_back(RAWINPUTDEVICE{
							.usUsagePage = info.hid.usUsagePage,
							.usUsage = info.hid.usUsage,
							.dwFlags = RIDEV_EXINPUTSINK,
							.hwndTarget = m_handle
						});
			}

			MYL_CORE_WARN("devices, {}", rids.size());
			if (!rids.empty())
				if (!RegisterRawInputDevices(rids.data(), static_cast<u32>(rids.size()), sizeof(RAWINPUTDEVICE)))
					throw core_runtime_error(std::format("Windows - Failed to register raw input devices: {}", GetLastError_as_string()));
		}
#endif
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
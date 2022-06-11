#include "win_window.hpp"
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/except.hpp>
#	include <mythos/core/log.hpp>
#	include <mythos/core/key_codes.hpp>
#	include <mythos/core/mouse_codes.hpp>
#	include <mythos/event/app_event.hpp>

#	include <mythos/input.hpp>

#	include <windowsx.h>

/// MYTodo: Figure out event_key_typed

namespace myl::windows {
	MYL_NO_DISCARD constexpr mouse_code translate_mouse_code(WPARAM w_param) {
		using namespace mouse_button;
		mouse_code mouse_buttons = none;
		if (w_param & MK_LBUTTON) mouse_buttons |= left;
		if (w_param & MK_RBUTTON) mouse_buttons |= right;
		if (w_param & MK_MBUTTON) mouse_buttons |= middle;
		if (w_param & MK_XBUTTON1) mouse_buttons |= button4;
		if (w_param & MK_XBUTTON2) mouse_buttons |= button5;

		return mouse_buttons;
	}

	MYL_NO_DISCARD static constexpr key_code translate_key_code(WPARAM w_param, LPARAM l_param) {
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

	LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
		switch (msg) {
			case WM_ERASEBKGND: // Notify the OS that erasing will be handled by the app to prevent flicker
				return 1;
			case WM_CLOSE: {
				event_window_close e{};
				fire_event(e);
			} return 0;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case WM_SIZE: { // Get the updated size
				RECT r;
				GetClientRect(hwnd, &r);
				event_window_resize e(static_cast<u32>(r.right - r.left), static_cast<u32>(r.bottom - r.top));
				fire_event(e);
			} break;
			case WM_KEYDOWN: MYL_FALLTHROUGH;
			case WM_SYSKEYDOWN:
				input::process_key(translate_key_code(w_param, l_param), input::state::down, static_cast<u32>(LOWORD(l_param))); // First 16 bits contain the repeat count
				break;
			case WM_KEYUP: MYL_FALLTHROUGH;
			case WM_SYSKEYUP:
				input::process_key(translate_key_code(w_param, l_param), input::state::up, 0);
				break;
			case WM_MOUSEMOVE:
				input::process_cursor_position(f32vec2(static_cast<f32>(GET_X_LPARAM(l_param)), static_cast<f32>(GET_Y_LPARAM(l_param))));
				break;
			case WM_MOUSEWHEEL: {
				i32 y_delta = GET_WHEEL_DELTA_WPARAM(w_param);
				if (y_delta != 0) // Flatten the input to an OS-independent(-1, 1)
					y_delta = y_delta < 0 ? -1 : 1;
				input::process_mouse_wheel({ 0.f, static_cast<f32>(y_delta) });
			} break;
			case WM_MOUSEHWHEEL: {
				i32 x_delta = GET_WHEEL_DELTA_WPARAM(w_param);
				if (x_delta != 0) // Flatten the input to an OS-independent(-1, 1)
					x_delta = x_delta < 0 ? -1 : 1;
				input::process_mouse_wheel({ static_cast<f32>(x_delta), 0.f });
			} break;
			case WM_LBUTTONDOWN: MYL_FALLTHROUGH;
			case WM_MBUTTONDOWN: MYL_FALLTHROUGH;
			case WM_RBUTTONDOWN: input::process_mouse_buttons(translate_mouse_code(w_param), input::state::down); break;
			case WM_XBUTTONDOWN: input::process_mouse_buttons(translate_mouse_code(LOWORD(w_param)), input::state::down); break; // LOWORD contains the button
#if 0 /// MYBug: For some reason w_param is always 0 when a button is released. Can't do stuff like block below
			case WM_LBUTTONUP: MYL_FALLTHROUGH;
			case WM_MBUTTONUP: MYL_FALLTHROUGH;
			case WM_RBUTTONUP: input::process_mouse_buttons(translate_mouse_code(w_param), input::state::up); break;
			case WM_XBUTTONUP: input::process_mouse_buttons(translate_mouse_code(LOWORD(w_param)), input::state::up); break; // LOWORD contains the button
#else /// MYHack: See bug above
			case WM_LBUTTONUP: input::process_mouse_buttons(mouse_button::left, input::state::up); break;
			case WM_MBUTTONUP: input::process_mouse_buttons(mouse_button::middle, input::state::up); break;
			case WM_RBUTTONUP: input::process_mouse_buttons(mouse_button::right, input::state::up); break;
			case WM_XBUTTONUP: input::process_mouse_buttons(HIWORD(w_param) == XBUTTON1 ? mouse_button::button4 : mouse_button::button5, input::state::up); break;
#endif
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
		WNDCLASSA wc;
		memset(&wc, 0, sizeof(wc));
		wc.style = CS_DBLCLKS;
		wc.lpfnWndProc = win32_process_message;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_instance;
		wc.hIcon = icon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = NULL;
		wc.lpszClassName = window_class_name;

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
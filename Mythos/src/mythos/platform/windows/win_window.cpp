#include "win_window.hpp"
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/except.hpp>
#	include <mythos/core/log.hpp>
#	include <mythos/core/timestep.hpp>

#	include <mythos/event/app_event.hpp>
#	include <mythos/event/key_event.hpp>
#	include <mythos/event/mouse_event.hpp>

#	include <windowsx.h>

namespace myl::windows {
	static window::event_callback* s_event_callback; /// MYHack: I think this is the proper way but not sure
	static void fire_event(event& a_event) {
		if (s_event_callback)
			(*s_event_callback)(a_event);
	}

	/// MYTodo: this should probs go into a windows input
	constexpr mouse_code translate_mouse_code(WPARAM w_param) {
		switch (w_param) {
			using namespace mouse_button;
			case MK_LBUTTON: return left;
			case MK_MBUTTON: return middle;
			case MK_RBUTTON: return right;
			default: return unknown;

				/// left + right = 3
				/// middle + left = 17
				/// middle + right = 18
				/// middle + left + right = 19
				/// MYBug: when 2 buttons are pressed at the same time it puts the values together, no way to handle that
				/// left = 0001; right = 0010; same time = 0011
				
				/// MYTodo: buttons on the sides on mouse
		}
	}

	constexpr key_code translate_key_code(WPARAM w_param) {
		switch (w_param) {
			using namespace key;
			default: return unknown;
		}
	}

	LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
		switch (msg) {
			case WM_ERASEBKGND: // notify the OS that erasing will be handled by the app to prevent flicker
				return 1;
			case WM_CLOSE: {
				window_close_event e{};
				fire_event(e);
			} return 0;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
			case WM_SIZE: { // get the updated size
				RECT r;
				GetClientRect(hwnd, &r);
				window_resize_event e(static_cast<u32>(r.right - r.left), static_cast<u32>(r.bottom - r.top));
				fire_event(e);
			} break;
			case WM_KEYDOWN: MYL_FALLTHROUGH;
			case WM_SYSKEYDOWN: {
				//key_pressed_event e(); /// code, repeats
				/// MYTodo: input processing
			} break;
			case WM_KEYUP: MYL_FALLTHROUGH;
			case WM_SYSKEYUP: {
				// key pressed / released
				//b8 pressed = msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN;
				//key_released_event e(); /// key code
				/// MYTodo: input processing
			} break;
			case WM_MOUSEMOVE: {
				mouse_moved_event e(static_cast<f32>(GET_X_LPARAM(l_param)), static_cast<f32>(GET_Y_LPARAM(l_param)));
				fire_event(e);
			} break;
			case WM_MOUSEWHEEL: {
				i32 y_delta = GET_WHEEL_DELTA_WPARAM(w_param);
				if (y_delta != 0) // flatten the input to an OS-independent(-1, 1)
					y_delta = y_delta < 0 ? -1 : 1;
				mouse_scrolled_event e(0.f, static_cast<f32>(y_delta));
				fire_event(e);
			} break;
			case WM_MOUSEHWHEEL: {
				i32 x_delta = GET_WHEEL_DELTA_WPARAM(w_param);
				if (x_delta != 0) // flatten the input to an OS-independent(-1, 1)
					x_delta = x_delta < 0 ? -1 : 1;
				mouse_scrolled_event e(static_cast<f32>(x_delta), 0.f);
				fire_event(e);
			} break;
			case WM_LBUTTONDOWN: MYL_FALLTHROUGH;
			case WM_MBUTTONDOWN: MYL_FALLTHROUGH;
			case WM_RBUTTONDOWN: {
				mouse_pressed_event e(translate_mouse_code(w_param));
				fire_event(e);
			} break;
			case WM_LBUTTONUP: { /// MYBug: for some reason w_param is always 0 when a button is released. Can't do stuff like above
				mouse_released_event e(mouse_button::left);
				fire_event(e);
			} break;
			case WM_MBUTTONUP: {
				mouse_released_event e(mouse_button::middle);
				fire_event(e);
			} break;
			case WM_RBUTTONUP: {
				mouse_released_event e(mouse_button::right);
				fire_event(e);
			} break;
			case WM_LBUTTONDBLCLK: MYL_FALLTHROUGH;
			case WM_MBUTTONDBLCLK: MYL_FALLTHROUGH;
			case WM_RBUTTONDBLCLK: {
				/// MYTodo: double clicks
			} break;
		}
	
		return DefWindowProcA(hwnd, msg, w_param, l_param);
	}

	window::window(const config& a_config)
		: myl::window() {
		m_instance = GetModuleHandleA(0);

		constexpr const char* window_class_name = "mythos_window_class";

		// setup and register window class
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

		if (!RegisterClassA(&wc)) {
			MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK); /// MYTodo: replace this with exception that will do this at main
			throw core_runtime_error("window registration failed");
		}

		// create window
		u32 client_x = a_config.x;
		u32 client_y = a_config.y;
		u32 client_width = a_config.width;
		u32 client_height = a_config.height;

		u32 window_x = client_x;
		u32 window_y = client_y;
		u32 window_width = client_width;
		u32 window_height = client_height;

		u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
		u32 window_ex_style = WS_EX_APPWINDOW;

		window_style |= WS_MAXIMIZE;
		window_style |= WS_MINIMIZEBOX;
		window_style |= WS_THICKFRAME;

		// obtain size of the border
		RECT border_rect = { 0, 0, 0, 0 };
		AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

		// in this case, the border rect is negative
		window_x += border_rect.left;
		window_y += border_rect.top;

		// grow by the size of the OS bordering
		window_width += border_rect.right - border_rect.left;
		window_height += border_rect.bottom - border_rect.top;

		m_handle = CreateWindowExA(window_ex_style, window_class_name, a_config.name.c_str(), window_style, window_x, window_y, window_width, window_height, 0, 0, m_instance, 0);
		if (!m_handle) {
			MessageBoxA(NULL, "Window creation failed", "Error!", MB_ICONEXCLAMATION | MB_OK); /// MYTodo: replace this with exception that will do this at main
			throw core_runtime_error("window creation failed");
		}

		// show the window
		bool should_activate = true; /// MYTodo: if the window should not acception input this should be false
		i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
		// if initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
		// if initially mazimized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE;
		ShowWindow(m_handle, show_window_command_flags);

		// clock setup
		setup_clock();

		// setup event callbacks
		s_event_callback = &m_event_callback;
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
}
#endif
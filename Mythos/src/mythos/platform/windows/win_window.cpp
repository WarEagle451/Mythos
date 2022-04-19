#include "win_window.hpp"
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/except.hpp>
#	include <mythos/core/log.hpp>
#	include <mythos/core/timestep.hpp>

#	include <windowsx.h>

namespace myl {
	static f64 s_clock_frequency; /// MYTodo: Probs should be in a diff file
	static LARGE_INTEGER s_start_time; /// MYTodo: Probs should be in a diff file

	f64 timestep::absolute() {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return static_cast<f64>(now.QuadPart * s_clock_frequency);
	}

	namespace windows {
		LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
			switch (msg) {
				case WM_ERASEBKGND: return 1; // notify the OS that erasing will be handled by the app to prevent flicker
				case WM_CLOSE: return 0; /// MYTodo: fire event for the app to quit
				case WM_DESTROY: 
					PostQuitMessage(0);
					return 0;
				case WM_SIZE: {
					// get the updated size
					// RECT r;
					// GetClientRect(hwnd, &r);
					// u32 width = r.right - r.left;
					// u32 height = r.bottom - r.top;

					/// MYTodo: fire resize window event
				} break;
				case WM_KEYDOWN: MYL_FALLTHROUGH;
				case WM_SYSKEYDOWN: MYL_FALLTHROUGH;
				case WM_KEYUP: MYL_FALLTHROUGH;
				case WM_SYSKEYUP: {
					// key pressed / released
					//b8 pressed = msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN;
					/// MYTodo: input processing
				} break;
				case WM_MOUSEMOVE: {
					// mouse move
					//i32 x_pos = GET_X_LPARAM(l_param);
					//i32 y_pos = GET_Y_LPARAM(l_param);
					/// MYTodo: input processing
				} break;
				case WM_MOUSEHWHEEL: {
					//i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
					//if (z_delta != 0) // flatten the input to an OS-independent(-1, 1)
					//	z_delta = z_delta < 0 ? -1 : 1;
					/// MYTodo: input processing
				} break;
				case WM_LBUTTONDOWN: MYL_FALLTHROUGH;
				case WM_MBUTTONDOWN: MYL_FALLTHROUGH;
				case WM_RBUTTONDOWN: MYL_FALLTHROUGH;
				case WM_LBUTTONUP: MYL_FALLTHROUGH;
				case WM_MBUTTONUP: MYL_FALLTHROUGH;
				case WM_RBUTTONUP: {
					//b8 pressed = msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN;
					/// MYTodo: input processing
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
				MYL_CORE_FATAL("window registration failed");
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
				MYL_CORE_FATAL("window creation failed");
				throw core_runtime_error("window creation failed");
			}

			// show the window
			b32 should_activate = 1; /// MYTodo: if the window should not acception input this should be false
			i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
			// if initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
			// if initially mazimized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE;
			ShowWindow(m_handle, show_window_command_flags);

			// clock setup
			LARGE_INTEGER frequency;
			QueryPerformanceCounter(&frequency);
			s_clock_frequency = 1.0 / static_cast<f64>(frequency.QuadPart);
			QueryPerformanceCounter(&s_start_time);
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
}
#endif
#pragma once
#include <mythos/platform/detection.hpp>
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/window.hpp>

#	include <windows.h>

namespace myl::windows {
	class window : public myl::window {
		HINSTANCE m_instance;
		HWND m_handle;

		event_callback m_event_callback;
	public:
		window(const config&);
		~window();

		window(const window&) = delete;
		window& operator=(const window&) = delete;

		MYL_API void* native() const override { return static_cast<void*>(m_handle); }
		//@return the module handle of the window
		HINSTANCE module() const { return m_instance; }

		void set_event_callback(const event_callback& a_callback) override { m_event_callback = a_callback; }

		void update() override;
	};
}
#endif
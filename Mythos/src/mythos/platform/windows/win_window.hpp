#pragma once
#include <mythos/platform/detection.hpp>
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/window.hpp>

#	include <windows.h>

namespace myl::windows {
	class window : public myl::window {
		HINSTANCE m_instance;
		HWND m_handle;
	public:
		window(const config&);
		~window();

		window(const window&) = delete;
		window& operator=(const window&) = delete;

		MYL_API MYL_NO_DISCARD void* native() const override { return static_cast<void*>(m_handle); }
		//@return Module handle of the window
		MYL_NO_DISCARD HINSTANCE module() const { return m_instance; }

		void update() override;
	};
}
#endif
#pragma once
#include <mythos/platform/detection.hpp>
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/window.hpp>

#	include <windows.h>

/// MYTodo: Should I replace all of this with GLFW?

namespace myl::windows {
	class window : public myl::window {
		HINSTANCE m_instance;
		HWND m_handle;
	public:
		MYL_API window(const config&);
		MYL_API ~window();

		window(const window&) = delete;
		window& operator=(const window&) = delete;

		MYL_API void* native() const override { return static_cast<void*>(m_handle); } /// MYTodo: is this the correct one?

		MYL_API void update() override;
	};
}
#endif
#pragma once
#include <mythos/platform/detection.hpp>
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/window.hpp>

#	include <windows.h>

namespace myl::windows {
	class window : public myl::window {
		HINSTANCE m_instance;
		HWND m_handle;

		u32vec2 m_size, m_cached_size{ 0, 0 };
		i32vec2 m_position_pre_fullscreen{ 0, 0 };
	public:
		window(const config&);
		~window();

		window(const window&) = delete;
		window& operator=(const window&) = delete;

		MYL_API MYL_NO_DISCARD void* native() const override { return static_cast<void*>(m_handle); }
		MYL_API MYL_NO_DISCARD const u32vec2& size() const override { return m_size; }
		//@return Module handle of the window
		MYL_NO_DISCARD HINSTANCE module() const { return m_instance; }

		void update() override;
		void on_resize(const u32vec2&) override;

		MYL_API void toggle_fullscreen() override;
	};
}
#endif
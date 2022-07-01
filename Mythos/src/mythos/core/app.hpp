#pragma once
#include "window.hpp"
#include "layer_stack.hpp"

#include <mythos/event/app_event.hpp>

#include <mythos/timer.hpp>

#include <string>
#include <memory>

namespace myl {
	struct app_info {
		std::string name = "Mythos Application";
		u32 major = 0;
		u32 minor = 0;
		u32 patch = 0;
	};

	class app {
		static app* s_instance;

		layer_stack m_layer_stack;
		std::unique_ptr<window> m_window;

		app_info m_info;

		bool m_running = true;
		bool m_suspended = false; // AKA minimized

		high_resolution_timer m_clock;
		std::chrono::nanoseconds m_last_frame_time;
		f64 m_target_frame_time;

		event::event_callback m_event_callback; // event callback needs to refer to a member
	public:
		struct config {
			window::config window{};
		};

		MYL_API MYL_NO_DISCARD static app& get() { return *s_instance; }

		MYL_API app(const app_info& = app_info{}, const config& = config{});
		MYL_API virtual ~app();

		MYL_API app(const app&) = delete;
		MYL_API app& operator=(const app&) = delete;

		MYL_API MYL_NO_DISCARD window* window() { return m_window.get(); }
		MYL_API MYL_NO_DISCARD app_info& info() { return m_info; }

		MYL_API void run();

		MYL_API void push_layer(layer_stack::layer_ptr);
		MYL_API void push_overlay(layer_stack::layer_ptr);

		MYL_API void close() { m_running = false; }
		MYL_API void set_fps(f64 a_fps) { m_target_frame_time = 1.0 / a_fps; }
	private:
		bool on_window_close(event::window_close&);
		bool on_window_resize(event::window_resize&);

		void on_event(event::event_base&);
	};
}
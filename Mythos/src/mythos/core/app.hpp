#pragma once
#include "window.hpp"
#include "layer_stack.hpp"

#include <string>
#include <memory>

namespace myl {
	class app {
		static app* s_instance;
		layer_stack m_layer_stack;
		std::unique_ptr<window> m_window;

		bool m_running = true;
		bool m_suspended = false; /// MYTodo: suspended like do nothing while minimized
		f64 m_last_frame_time = 0.0;
	public:
		struct config {
			i16 x, y;
			i16 width, height;
			std::string name;
		};

		MYL_API static app& get() { return *s_instance; }

		MYL_API app(const config&);
		MYL_API ~app();

		MYL_API app(const app&) = delete;
		MYL_API app& operator=(const app&) = delete;

		MYL_API void close() { m_running = false; }

		MYL_API void run();

		MYL_API void push_layer(layer_stack::layer_ptr a_layer);
		MYL_API void push_overlay(layer_stack::layer_ptr a_overlay);
	};
}
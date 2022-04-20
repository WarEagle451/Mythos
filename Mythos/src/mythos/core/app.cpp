#include "app.hpp"
#include "assert.hpp"
#include "log.hpp"

namespace myl {
	app* app::s_instance = nullptr;

	app::app(const config& a_config) {
		core::loggers::init(); // asserts contains a call to MYL_CORE_FATAL and does not need to be exported here
		MYL_CORE_ASSERT(s_instance == nullptr, "app has already been created");
		s_instance = this;

		m_running = true;
		m_window = window::create(window::config{  a_config.x, a_config.y, a_config.width, a_config.height, a_config.name });
	}

	app::~app() {

	}

	void app::run() {
		while (m_running) {
			f64 time = get_time();
			timestep ts = time - m_last_frame_time;

			while (ts < 1.0 / 60.0) { /// MYHack: do vsync the right way
				time = get_time();
				ts = time - m_last_frame_time;
			}

			m_last_frame_time = time;

			if (!m_suspended) {
				for (auto& l : m_layer_stack) l->update(ts);
				for (auto& l : m_layer_stack) l->render();
			}

			m_window->update();
		}
	}

	void app::push_layer(layer_stack::layer_ptr a_layer) {
		m_layer_stack.push_layer(std::move(a_layer));
	}

	void app::push_overlay(layer_stack::layer_ptr a_overlay) {
		m_layer_stack.push_overlay(std::move(a_overlay));
	}
}
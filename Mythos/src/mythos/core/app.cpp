#include "app.hpp"
#include "assert.hpp"
#include "log.hpp"
#include "input.hpp"

#include <mythos/event/app_event.hpp>

/// MYTodo: will want to have different event callbacks than just on_event

namespace myl {
	app* app::s_instance = nullptr;

	app::app(const config& a_config) {
		core::loggers::init(); // asserts contain a call to MYL_CORE_FATAL
		MYL_CORE_ASSERT(s_instance == nullptr, "Application has already been created");

		MYL_CORE_INFO("Mythos version: {}", MYL_VERSION);

		s_instance = this;
		m_running = true;

		input::internal_states::init(); // init key states

		m_window = window::create(a_config.window);
		m_window->set_event_callback(MYL_BIND_EVENT_FN(app::on_event)); /// MYTodo: why pass this though window? why not just set it from here?

		/// MYTodo: Init renderer
		/// MYTodo: Init audio
	}

	app::~app() {
		MYL_CORE_INFO("Shutting down application");
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

				input::internal_states::update();
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

	bool app::on_window_close(event_window_close& a_event) {
		close();
		return true;
	}

	bool app::on_window_resize(event_window_resize& a_event) {
		if (a_event.width() == 0 || a_event.height() == 0) {
			m_suspended = true;
			return false;
		}
		MYL_CORE_INFO("window resized: [{}, {}]", a_event.width(), a_event.height()); /// MYTemp: for testing events
		m_suspended = false;
		return false;
	}

	void app::on_event(event& a_event) {
		event_dispatcher dispatcher(a_event);
		dispatcher.dispatch<event_window_close>(MYL_BIND_EVENT_FN(app::on_window_close));
		dispatcher.dispatch<event_window_resize>(MYL_BIND_EVENT_FN(app::on_window_resize));

		for (auto& layer : m_layer_stack) {
			if (a_event.handled)
				break;
			layer->on_event(a_event);
		}
	}
}
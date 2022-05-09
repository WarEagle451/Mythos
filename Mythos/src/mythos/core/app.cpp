#include "app.hpp"
#include "assert.hpp"
#include "log.hpp"
#include "input.hpp"

#include <mythos/event/app_event.hpp>
#include <mythos/render/renderer.hpp>

#include <deque>

namespace myl {
	app* app::s_instance = nullptr;

	app::app(const app_info& a_info, const config& a_config)
		: m_info(a_info) {
		core::loggers::init(); // Asserts contain a call to MYL_CORE_FATAL
		MYL_CORE_INFO("Creating application");
		MYL_CORE_ASSERT(s_instance == nullptr, "Application has already been created");
		s_instance = this;

		MYL_CORE_INFO("Mythos version: {}", MYL_VERSION);

		m_running = true;

		input::internal_states::init();

		m_window = window::create(a_config.window);
		m_window->set_event_callback(MYL_BIND_EVENT_FN(app::on_event)); /// MYTodo: Why pass this though window? why not just set it from here?

		render::renderer::init();
		MYL_CORE_INFO("Application initialized");
	}

	app::~app() {
		MYL_CORE_INFO("Terminating application");
		m_layer_stack.clear(); // Client layers are destroyed before any internal systems are shutdown

		render::renderer::shutdown();
		m_window.reset();
		MYL_CORE_INFO("Application terminated");
	}

	void app::run() {
		while (m_running) {
			f64 time = get_time();
			timestep ts = time - m_last_frame_time;
			m_last_frame_time = time;

			if (!m_suspended) {
				for (auto& l : m_layer_stack) l->update(ts);
				for (auto& l : m_layer_stack) l->render();

				render::renderer::draw_frame();
				input::internal_states::update();
			}

			m_window->update();

			if (!m_suspended) {
				static constexpr f64 target_frame_time = 1.0 / 60.0;
				f64 delta = get_time() - m_last_frame_time;

				while (delta < target_frame_time) /// MYHack: Do vsync the right way, this way is resource intensive
					delta = get_time() - m_last_frame_time;
			}
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
		/// MYTodo: This should return true if somehow the width and height don't actually change
		if (a_event.size().x == 0 || a_event.size().y == 0) { // Minimization
			m_suspended = true;
			return false;
		}

		m_suspended = false;
		render::renderer::on_window_resize(a_event.size());
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
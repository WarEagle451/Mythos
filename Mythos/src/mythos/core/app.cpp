#include "app.hpp"
#include "assert.hpp"
#include "log.hpp"

#include <mythos/input.hpp>
#include <mythos/event/app_event.hpp>
#include <mythos/render/renderer.hpp>

namespace myl {
	app* app::s_instance = nullptr;

	app::app(const app_info& a_info, const config& a_config)
		: m_info(a_info) {
		loggers::init(); // Asserts contain a call to MYL_CORE_FATAL
		MYL_CORE_INFO("Creating application");
		MYL_CORE_ASSERT(s_instance == nullptr, "Application has already been created");
		s_instance = this;
		MYL_CORE_INFO("Mythos:");
		MYL_CORE_INFO("\t- Version: {}", MYL_VERSION_STRING);
		/// MYTodo: Should probs output config info

		input::init();
		m_window = window::create(a_config.window);

		m_event_callback = MYL_BIND_EVENT_FN(app::on_event);
		set_event_callback(m_event_callback);

		render::renderer::init();
		MYL_CORE_INFO("Application initialized");

		m_running = true;
	}

	app::~app() {
		MYL_CORE_INFO("Terminating application");
		m_layer_stack.clear(); // Client layers are destroyed before any internal systems are shutdown

		render::renderer::shutdown();
		m_window.reset();
		MYL_CORE_INFO("Terminated application");
	}

	void app::run() {
		m_clock.reset(); // Start time needs to be since app started running

		while (m_running) {
			std::chrono::nanoseconds time = m_clock.elapsed();
			timestep ts = std::chrono::duration<f64, std::chrono::seconds::period>(time - m_last_frame_time).count();
			m_last_frame_time = time;

			if (!m_suspended) {
				for (auto& l : m_layer_stack) l->update(ts);
				for (auto& l : m_layer_stack) l->render();

				input::update(); // Called after because it clears current values for next frame
			}

			m_window->update();
			
			if (!m_suspended) {
				static constexpr std::chrono::nanoseconds target_frame_time(static_cast<i64>((1.0 / 60.0) * 1'000'000'000));
				std::chrono::nanoseconds time_remaining = target_frame_time - (m_clock.elapsed() - m_last_frame_time);
				while (time_remaining.count() > 0)
					time_remaining = target_frame_time - (m_clock.elapsed() - m_last_frame_time);
				/// MYBug: Why is this limiting at ~33 fps
				/// MYTodo: Do vsync the right way
				///if (time_remaining.count() > 0)
				///	std::this_thread::sleep_for(time_remaining);
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
		if (m_window->size() == u32vec2{ 0, 0 })
			return true; // Don't trigger a resize if no change

		if (a_event.size().x == 0 || a_event.size().y == 0) { // Minimization
			m_suspended = true;
			return false;
		}
		
		m_suspended = false;
		m_window->on_resize(a_event.size());
		render::renderer::on_window_resize(a_event.size());
		return false;
	}

	void app::on_event(event& a_event) { /// MYTodo: Instead of this have listeners that have a function pointer, this will also allow them to unsubscribe, Would this be faster? 
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
#include "testbed_layer.hpp"

#include <mythos/core/log.hpp>
#include <mythos/event/event.hpp>
#include <mythos/event/mouse_event.hpp>

namespace tb {
	static bool mouse_moved(myl::mouse_moved_event& e) {
		MYL_CORE_INFO("mouse cords: [{}, {}]", e.x(), e.y());
		return true;
	}

	static bool mouse_scrolled(myl::mouse_scrolled_event& e) {
		MYL_CORE_INFO("mouse delta: [{}, {}]", e.x_delta(), e.y_delta());
		return true;
	}

	static bool mouse_pressed(myl::mouse_pressed_event& e) {
		MYL_CORE_INFO("mouse pressed: {}", static_cast<i32>(e.button()));
		return true;
	}

	static bool mouse_released(myl::mouse_released_event& e) {
		MYL_CORE_INFO("mouse released: {}", static_cast<i32>(e.button()));
		return true;
	}

	testbed_layer::testbed_layer()
		: myl::layer("Testbed") {
		MYL_CORE_INFO("Testbed layer created");
	}

	testbed_layer::~testbed_layer() {
		MYL_CORE_INFO("Testbed layer destroyed");
	}

	void testbed_layer::on_attach() {
		MYL_CORE_INFO("Testbed layer on_attach");
	}

	void testbed_layer::on_detach() {
		MYL_CORE_INFO("Testbed layer on_detach");
	}

	void testbed_layer::on_event(myl::event& a_event) {
		myl::event_dispatcher dispatcher(a_event);
		dispatcher.dispatch<myl::mouse_moved_event>(mouse_moved);
		dispatcher.dispatch<myl::mouse_scrolled_event>(mouse_scrolled);
		dispatcher.dispatch<myl::mouse_pressed_event>(mouse_pressed);
		dispatcher.dispatch<myl::mouse_released_event>(mouse_released);
	}

	void testbed_layer::update(myl::timestep ts) {

	}

	void testbed_layer::render() {

	}
}
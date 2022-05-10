#include "testbed_layer.hpp"

#include <mythos/core/log.hpp>
#include <mythos/event/event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/event/key_event.hpp>

#include <mythos/core/input.hpp>

#include <mythos/core/app.hpp>

namespace tb {
	static bool mouse_pressed(myl::event_mouse_pressed& e) {
		MYL_CORE_TRACE("mouse pressed: {}", e.buttons());
		return true;
	}

	static bool mouse_released(myl::event_mouse_released& e) {
		MYL_CORE_TRACE("mouse released: {}", e.buttons());
		return true;
	}

	static bool key_released(myl::event_key_released& e) {
		MYL_CORE_TRACE("Key '{}' up", e.key());
		return true;
	}

	static bool key_pressed(myl::event_key_pressed& e) {
		MYL_CORE_TRACE("Key '{}' down", e.key());
		return true;
	}

	testbed_layer::testbed_layer()
		: myl::layer("Testbed") {
		MYL_CORE_TRACE("Testbed layer created");
	}

	testbed_layer::~testbed_layer() {
		MYL_CORE_TRACE("Testbed layer destroyed");
	}

	void testbed_layer::on_attach() {
		MYL_CORE_TRACE("Testbed layer on_attach");
	}

	void testbed_layer::on_detach() {
		MYL_CORE_TRACE("Testbed layer on_detach");
	}

	void testbed_layer::on_event(myl::event& a_event) {
		myl::event_dispatcher dispatcher(a_event);
		dispatcher.dispatch<myl::event_mouse_pressed>(mouse_pressed);
		dispatcher.dispatch<myl::event_mouse_released>(mouse_released);
		dispatcher.dispatch<myl::event_key_pressed>(key_pressed);
		dispatcher.dispatch<myl::event_key_released>(key_released);
	}

	void testbed_layer::update(myl::timestep ts) {
		if (myl::input::key_down(myl::key::escape))
			myl::app::get().close();
	}

	void testbed_layer::render() {

	}
}
#include "testbed_layer.hpp"

#include <mythos/core/log.hpp>
#include <mythos/core/app.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/input.hpp>

#include <testbed/utils/keys_buttons_string.hpp>

#include "test_manager.hpp"

#include <mythos/io.hpp>

#define TESTBED_RUN_TESTS 0
#define TESTBED_ENABLE_EVENT_TESTS 0

namespace tb {
	static bool key_pressed(myl::event_key_pressed& e) {
		MYL_CORE_TRACE("Key '{}' pressed", key_to_string(e.key()));
		return true;
	}

	static bool key_released(myl::event_key_released& e) {
		MYL_CORE_TRACE("Key '{}' released", key_to_string(e.key()));
		return true;
	}

	static bool mouse_pressed(myl::event_mouse_pressed& e) {
		MYL_CORE_TRACE("Mouse button(s) '{}' pressed", mouse_buttons_to_string(e.buttons()));
		return true;
	}

	static bool mouse_released(myl::event_mouse_released& e) {
		MYL_CORE_TRACE("Mouse button(s) '{}' released", mouse_buttons_to_string(e.buttons()));
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
#if TESTBED_RUN_TESTS
		test_manager tests;
		tests.run();
#endif
	}

	void testbed_layer::on_detach() {
		MYL_CORE_TRACE("Testbed layer on_detach");
	}

	void testbed_layer::on_event(myl::event& a_event) {
		myl::event_dispatcher dispatcher(a_event);
#if TESTBED_ENABLE_EVENT_TESTS
		dispatcher.dispatch<myl::event_key_pressed>(key_pressed);
		dispatcher.dispatch<myl::event_key_released>(key_released);
		dispatcher.dispatch<myl::event_mouse_pressed>(mouse_pressed);
		dispatcher.dispatch<myl::event_mouse_released>(mouse_released);
#endif
	}

	void testbed_layer::update(myl::timestep ts) {
		if (myl::input::key_down(myl::key::escape) && myl::input::key_down(myl::key::left_shift))
			myl::app::get().close();
	}

	void testbed_layer::render() {
	}
}
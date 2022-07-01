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
#define TESTBED_ENABLE_EVENT_TESTS 1

namespace tb {
	static bool key_typed(myl::event::key_typed& e) {
		MYL_CORE_DEBUG("Key typed: '{}'", static_cast<char>(e.key()));
		return true;
	}

	static bool key_pressed(myl::event::key_pressed& e) {
		MYL_CORE_TRACE("Key '{}' pressed", key_to_string(e.key()));
		return true;
	}

	static bool key_released(myl::event::key_released& e) {
		MYL_CORE_TRACE("Key '{}' released", key_to_string(e.key()));
		return true;
	}

	static bool mouse_pressed(myl::event::mouse_pressed& e) {
		MYL_CORE_TRACE("Mouse button(s) '{}' pressed", mouse_buttons_to_string(e.buttons()));
		return true;
	}

	static bool mouse_released(myl::event::mouse_released& e) {
		MYL_CORE_TRACE("Mouse button(s) '{}' released", mouse_buttons_to_string(e.buttons()));
		return true;
	}

	static bool mouse_scrolled(myl::event::mouse_scrolled& e) {
		MYL_CORE_TRACE("Mouse scrolled: [{}, {}]", e.delta().x, e.delta().y);
		return true;
	}

	static bool mouse_moved(myl::event::mouse_moved& e) {
		MYL_CORE_TRACE("Mouse moved: [{}, {}]", e.position().x, e.position().y);
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

	void testbed_layer::on_event(myl::event::event_base& a_event) {
		myl::event::dispatcher dispatcher(a_event);
#if TESTBED_ENABLE_EVENT_TESTS
		dispatcher.dispatch<myl::event::key_pressed>(key_pressed);
		dispatcher.dispatch<myl::event::key_released>(key_released);
		dispatcher.dispatch<myl::event::mouse_pressed>(mouse_pressed);
		dispatcher.dispatch<myl::event::mouse_released>(mouse_released);
		dispatcher.dispatch<myl::event::mouse_scrolled>(mouse_scrolled);
		dispatcher.dispatch<myl::event::mouse_moved>(mouse_moved);
		dispatcher.dispatch<myl::event::key_typed>(key_typed);
#endif
	}

	void testbed_layer::update(myl::timestep ts) {
		if (myl::input::key_down(myl::key::escape) && myl::input::key_down(myl::key::left_shift))
			myl::app::get().close();

		if (myl::input::key_released(myl::key::f1))
			myl::app::get().window()->toggle_fullscreen();
	}

	void testbed_layer::render() {

	}
}
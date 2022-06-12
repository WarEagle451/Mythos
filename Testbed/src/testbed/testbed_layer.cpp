#include "testbed_layer.hpp"

#include <mythos/core/log.hpp>
#include <mythos/core/app.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/input.hpp>

#include <testbed/utils/keys_buttons_string.hpp>

#include "test_manager.hpp"

#include <mythos/file.hpp>

///#define TESTBED_RUN_TESTS

namespace tb {
	testbed_layer::testbed_layer()
		: myl::layer("Testbed") {
		MYL_CORE_TRACE("Testbed layer created");
	}

	testbed_layer::~testbed_layer() {
		MYL_CORE_TRACE("Testbed layer destroyed");
	}

	void testbed_layer::on_attach() {
		MYL_CORE_TRACE("Testbed layer on_attach");
#ifdef TESTBED_RUN_TESTS
		test_manager tests;
		tests.run();
#endif
	}

	void testbed_layer::on_detach() {
		MYL_CORE_TRACE("Testbed layer on_detach");
	}

	void testbed_layer::on_event(myl::event& a_event) {
		myl::event_dispatcher dispatcher(a_event);
	}

	void testbed_layer::update(myl::timestep ts) {
		if (myl::input::key_down(myl::key::escape) && myl::input::key_down(myl::key::left_shift))
			myl::app::get().close();
	}

	void testbed_layer::render() {
	}
}
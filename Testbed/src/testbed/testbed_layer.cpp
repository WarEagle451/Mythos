#include "testbed_layer.hpp"

#include <mythos/core/app.hpp>
#include <mythos/core/log.hpp>
#include <mythos/core/assert.hpp>

namespace tb {
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

	void testbed_layer::update(myl::timestep ts) {

	}

	void testbed_layer::render() {

	}
}
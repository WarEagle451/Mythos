#include <mythos/core/entry.hpp>

#include "testbed/testbed_layer.hpp"

namespace myl {
	std::unique_ptr<app> create() {
		app::config config;
		config.window.name = "Testbed";

		auto application = std::make_unique<app>(config);
		application->push_layer(new tb::testbed_layer());
		return application;
	}
}
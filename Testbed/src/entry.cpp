#include <mythos/core/entry.hpp>

#include "testbed/testbed_layer.hpp"

namespace myl {
	std::unique_ptr<app> create() {
		std::unique_ptr<app> application = std::make_unique<app>(app::config{ 100, 100, 1280, 720, "Testbed" });
		application->push_layer(new tb::testbed_layer());
		return application;
	}
}
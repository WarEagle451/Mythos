#include <mythos/core/entry.hpp>

#include "testbed/testbed_layer.hpp"

#define TESTBED_MAJOR 1
#define TESTBED_MINOR 0
#define TESTBED_PATCH 0

namespace myl {
	std::unique_ptr<app> create() {
		app::config config{
			.window = {
				.name = "Testbed"
			}
		};

		app_info info{
			.name = "Testbed",
			.major = TESTBED_MAJOR,
			.minor = TESTBED_MINOR,
			.patch = TESTBED_PATCH
		};

		auto application = std::make_unique<app>(info, config);
		application->push_layer(new tb::testbed_layer());
		return application;
	}
}
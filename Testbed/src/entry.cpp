#include "testbed/game_layer.hpp"
#include "testbed/testbed_layer.hpp"

#include <mythos/core/entry.hpp>

#define TESTBED_MAJOR 1
#define TESTBED_MINOR 0
#define TESTBED_PATCH 0

#define TESTBED_NAME "Testbed"

namespace tb {
	class testbed_app : public myl::app {
	public:
		testbed_app(const myl::app_info& a_info, const myl::app::config& a_config)
			: myl::app(a_info, a_config) {
			push_layer(new testbed_layer());
			push_layer(new game_layer());
		}

		~testbed_app() {}
	};
}

namespace myl {
	std::unique_ptr<app> create() {
		app_info info{
			.name = TESTBED_NAME,
			.major = TESTBED_MAJOR,
			.minor = TESTBED_MINOR,
			.patch = TESTBED_PATCH
		};

		app::config config{
			.window = {
				.style = myl::window_style::windowed,
				.name = TESTBED_NAME
			}
		};

		return std::make_unique<tb::testbed_app>(info, config);
	}
}
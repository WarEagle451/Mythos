#include "testbed/testbed_layer.hpp"
#include "testbed/game_layer.hpp"

#include <mythos/core/entry.hpp>

#define TESTBED_MAJOR 1
#define TESTBED_MINOR 0
#define TESTBED_PATCH 0

#define TESTBED_NAME "Testbed"

namespace tb {
	class testbed_app : public myl::app {
	public:
		testbed_app(const myl::app::config& a_config)
			: myl::app(get_info(), a_config) {
			push_layer(new testbed_layer());
			push_layer(new game_layer());
		}

		~testbed_app() {

		}
	private:
		myl::app_info get_info() {
			return myl::app_info{
				.name = TESTBED_NAME,
				.major = TESTBED_MAJOR,
				.minor = TESTBED_MINOR,
				.patch = TESTBED_PATCH
			};
		}
	};
}

namespace myl {
	std::unique_ptr<app> create() {
		app::config config{
			.window = {
				.name = TESTBED_NAME
			}
		};

		return std::make_unique<tb::testbed_app>(config);
	}
}
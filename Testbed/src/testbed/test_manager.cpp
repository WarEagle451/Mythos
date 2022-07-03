#include "tests/math.hpp"
#include "test_manager.hpp"

namespace tb {
	test_manager::test_manager() {
		add_math_tests(*this);
	}
}
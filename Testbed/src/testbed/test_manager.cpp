#include "test_manager.hpp"

#include "tests/math.hpp"

namespace tb {
	test_manager::test_manager() {
		add_math_tests(*this);
	}
}
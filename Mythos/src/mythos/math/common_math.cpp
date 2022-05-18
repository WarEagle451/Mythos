#include "common.hpp"

#include <cmath> ///MYTemp:
#include <limits> /// MYTemp:

namespace myl { /// MYTodo: This file shouldn't exist. Hides std::functions
	/// MYTodo: Probs best to have a very small number
	constexpr bool approx(f32 a, f32 b) { return myl::abs(a - b) <= ((myl::abs(a) < myl::abs(b) ? myl::abs(b) : myl::abs(a)) * std::numeric_limits<f32>::epsilon()); }
	constexpr bool approx(f64 a, f64 b) { return myl::abs(a - b) <= ((myl::abs(a) < myl::abs(b) ? myl::abs(b) : myl::abs(a)) * std::numeric_limits<f64>::epsilon()); }

	inline f32 sqrt(f32 value) { return std::sqrt(value); }
	inline f64 sqrt(f64 value) { return std::sqrt(value); }
}
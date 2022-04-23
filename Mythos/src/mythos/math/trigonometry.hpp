#pragma once
#include <mythos/defines.hpp>

#include <numbers>

namespace myl::math {
	template<typename T>
	MYL_NO_DISCARD constexpr T to_degrees(T a_radian) {
		return (static_cast<T>(180) / std::numbers::pi_v<T>) * a_radian;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T to_radians(T a_degree) {
		return a_degree * (std::numbers::pi_v<T> / static_cast<T>(180));
	}
}
#pragma once
#include <mythos/defines.hpp>

namespace myl::math {
	template<typename T>
	MYL_NO_DISCARD constexpr T min(T a, T b) {
		return a > b ? b : a;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T max(T a, T b) {
		return a > b ? a : b;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T floor(T a_value, T a_floor) {
		return a_value > a_floor ? a_value : a_floor;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T ceil(T a_value, T a_ceil) {
		return a_value > a_ceil ? a_ceil : a_value;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T difference(T a, T b) {
		return a > b ? a - b : b - a;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T lerp(const T a, const T b, const float t = .5f) {
		return a + (b - a) * t;
	}
}
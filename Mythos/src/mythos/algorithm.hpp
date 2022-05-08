#pragma once
#include <mythos/defines.hpp>

namespace myl {
	template<typename T>
	MYL_NO_DISCARD constexpr T min(const T a, const T b) {
		return a > b ? b : a;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T max(const T a, const T b) {
		return a > b ? a : b;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T floor(const T value, const T floor) {
		return value > floor ? value : floor;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T ceil(const T value, const T ceil) {
		return value > ceil ? ceil : value;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T difference(const T a, const T b) {
		return a > b ? a - b : b - a;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T lerp(const T a, const T b, const f32 percentage = .5f) {
		return a + (b - a) * percentage;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T clamp(const T value, const T min, const T max) {
		return value < min ? min : value > max ? max : value;
	}
}
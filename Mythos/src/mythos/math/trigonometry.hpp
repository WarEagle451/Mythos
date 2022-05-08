#pragma once
#include <mythos/defines.hpp>

#include <numbers>

#include <cmath> /// MYTodo: Get rid of this

namespace myl {
	template<typename T> MYL_NO_DISCARD constexpr T degrees(T radian) { return (static_cast<T>(180) / std::numbers::pi_v<T>) * radian; }
	template<typename T> MYL_NO_DISCARD constexpr T radians(T degree) { return degree * (std::numbers::pi_v<T> / static_cast<T>(180)); }

	template<typename T> MYL_NO_DISCARD constexpr T sin(T radian) { return std::sin(radian); }
	template<typename T> MYL_NO_DISCARD constexpr T cos(T radian) { return std::cos(radian); }
	template<typename T> MYL_NO_DISCARD constexpr T tan(T radian) { return std::tan(radian); }

	template<typename T> MYL_NO_DISCARD constexpr T asin(T arc) { return std::asin(arc); }
	template<typename T> MYL_NO_DISCARD constexpr T acos(T arc) { return std::acos(arc); }
	template<typename T> MYL_NO_DISCARD constexpr T atan(T arc) { return std::atan(arc); }
}
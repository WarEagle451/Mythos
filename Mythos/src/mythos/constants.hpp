#pragma once
namespace myl::constants {
	template<typename T> inline constexpr T phi					= static_cast<T>(1.61803398874989484820);
	template<typename T> inline constexpr T egamma				= static_cast<T>(1.78107241799019798523);
	template<typename T> inline constexpr T e					= static_cast<T>(2.71828182845904523536);

	template<typename T> inline constexpr T inverse_pi			= static_cast<T>(0.31830988618379067153);
	template<typename T> inline constexpr T two_over_pi			= static_cast<T>(0.63661977236758134308);
	template<typename T> inline constexpr T quater_pi			= static_cast<T>(0.78539816339744830961);
	template<typename T> inline constexpr T half_pi				= static_cast<T>(1.57079632679489661923);
	template<typename T> inline constexpr T pi					= static_cast<T>(3.14159265358979323846);
	template<typename T> inline constexpr T two_pi				= static_cast<T>(6.28318530717958647692);

	template<typename T> inline constexpr T inverse_sqrt_three	= static_cast<T>(0.57735026918962576450);
	template<typename T> inline constexpr T inverse_sqrt_two	= static_cast<T>(0.70710678118654752440);
	template<typename T> inline constexpr T sqrt_two			= static_cast<T>(1.41421356237309504880);
	template<typename T> inline constexpr T sqrt_three			= static_cast<T>(1.73205080756887729352);

	/// MYTodo: epsilion, infinity
}
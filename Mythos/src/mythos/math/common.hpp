#pragma once
#include <mythos/defines.hpp>

namespace myl {
	/// MYTodo: Template functions
	
	template<typename T> MYL_NO_DISCARD T abs(T) noexcept;

	MYL_API MYL_NO_DISCARD constexpr bool approx(i8, i8, i8 flexibility = 1);
	MYL_API MYL_NO_DISCARD constexpr bool approx(i16, i16, i16 flexibility = 1);
	MYL_API MYL_NO_DISCARD constexpr bool approx(i32, i32, i32 flexibility = 1);
	MYL_API MYL_NO_DISCARD constexpr bool approx(i64, i64, i64 flexibility = 1);
	MYL_API MYL_NO_DISCARD constexpr bool approx(u8, u8, u8 flexibility = 1);
	MYL_API MYL_NO_DISCARD constexpr bool approx(u16, u16, u16 flexibility = 1);
	MYL_API MYL_NO_DISCARD constexpr bool approx(u32, u32, u32 flexibility = 1);
	MYL_API MYL_NO_DISCARD constexpr bool approx(u64, u64, u64 flexibility = 1);
	MYL_API MYL_NO_DISCARD constexpr bool approx(f32, f32);
	MYL_API MYL_NO_DISCARD constexpr bool approx(f64, f64);

	/// MYTodo: Constexpr
	MYL_API MYL_NO_DISCARD inline f32 sqrt(f32);
	MYL_API MYL_NO_DISCARD inline f64 sqrt(f64);

	template<typename FloatT, typename T>
	MYL_NO_DISCARD constexpr FloatT lerp(T a, T b, FloatT percentage) {
		return static_cast<FloatT>(a) + static_cast<FloatT>(b - a) * percentage;
	}

	template<typename FloatT = f32, typename T>
	MYL_NO_DISCARD constexpr FloatT inverse_lerp(T a, T b, T value) {
		return static_cast<FloatT>(value - a) / static_cast<FloatT>(b - a);
	}
}
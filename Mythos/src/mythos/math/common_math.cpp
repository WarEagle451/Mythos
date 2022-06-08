#include "common.hpp"

#include <cmath> ///MYTemp:
#include <limits> /// MYTemp:

namespace myl {
	constexpr i8 abs(i8 value) noexcept		{ return value < 0 ? -value : value; }
	constexpr i16 abs(i16 value) noexcept	{ return value < 0 ? -value : value; }
	constexpr i32 abs(i32 value) noexcept	{ return value < 0 ? -value : value; }
	constexpr i64 abs(i64 value) noexcept	{ return value < 0 ? -value : value; }
	constexpr f32 abs(f32 value) noexcept	{ return value < 0.f ? -value : value; }
	constexpr f64 abs(f64 value) noexcept	{ return value < 0.0 ? -value : value; }

	constexpr bool approx(i8 a, i8 b, i8 flexibility)		{ return a - flexibility <= b && b <= a + flexibility; }
	constexpr bool approx(i16 a, i16 b, i16 flexibility)	{ return a - flexibility <= b && b <= a + flexibility; }
	constexpr bool approx(i32 a, i32 b, i32 flexibility)	{ return a - flexibility <= b && b <= a + flexibility; }
	constexpr bool approx(i64 a, i64 b, i64 flexibility)	{ return a - flexibility <= b && b <= a + flexibility; }
	constexpr bool approx(u8 a, u8 b, u8 flexibility)		{ return a - flexibility <= b && b <= a + flexibility; }
	constexpr bool approx(u16 a, u16 b, u16 flexibility)	{ return a - flexibility <= b && b <= a + flexibility; }
	constexpr bool approx(u32 a, u32 b, u32 flexibility)	{ return a - flexibility <= b && b <= a + flexibility; }
	constexpr bool approx(u64 a, u64 b, u64 flexibility)	{ return a - flexibility <= b && b <= a + flexibility; }

	constexpr bool approx(f32 a, f32 b) { return myl::abs(a - b) <= ((myl::abs(a) < myl::abs(b) ? myl::abs(b) : myl::abs(a)) * std::numeric_limits<f32>::epsilon()); }
	constexpr bool approx(f64 a, f64 b) { return myl::abs(a - b) <= ((myl::abs(a) < myl::abs(b) ? myl::abs(b) : myl::abs(a)) * std::numeric_limits<f64>::epsilon()); }

	inline f32 sqrt(f32 value) { return std::sqrt(value); }
	inline f64 sqrt(f64 value) { return std::sqrt(value); }
}
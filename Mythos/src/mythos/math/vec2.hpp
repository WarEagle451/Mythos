#pragma once
#include <mythos/defines.hpp>

#include "structure_def.hpp"

#include <utility> // std::move

namespace myl {
	template<typename T>
	struct vec<2, T> {
		using value_type = T;

		union {
			value_type data[2];
			struct { value_type x, y; };
			struct { value_type r, g; };
			struct { value_type s, t; };
			struct { value_type u, v; };
			struct { value_type w, h; };
		};

		// Constructors

		constexpr vec() = default;

		constexpr explicit vec(const value_type& a_scalar)
			: data{ a_scalar, a_scalar } {}

		constexpr vec(const value_type& a_x, const value_type& a_y)
			: data{ a_x, a_y } {}

		constexpr vec(value_type&& a_x, value_type&& a_y)
			: data{ std::move(a_x), std::move(a_y) } {}

		// Utilities

		MYL_NO_DISCARD static constexpr vec zero() { return vec{ 0, 0 }; }
		MYL_NO_DISCARD static constexpr vec one() { return vec{ 1, 1 }; }

		// Access operators

		MYL_NO_DISCARD constexpr value_type& operator[](std::size_t i) { return data[i]; }
		MYL_NO_DISCARD constexpr const value_type& operator[](std::size_t i) const { return data[i]; }

		// Comparison operators

		MYL_NO_DISCARD constexpr bool operator==(const vec& rhs) const { return x == rhs.x && y = rhs.y; }
		/// spaceship?

		// Unary operators

		MYL_NO_DISCARD constexpr vec operator-() const { return vec{ -x, -y }; }

		// Scaler operators

		MYL_NO_DISCARD constexpr vec operator+(const value_type& rhs) const { return vec(x + rhs, y + rhs); }
		MYL_NO_DISCARD constexpr vec operator-(const value_type& rhs) const { return vec(x - rhs, y - rhs); }
		MYL_NO_DISCARD constexpr vec operator*(const value_type& rhs) const { return vec(x * rhs, y * rhs); }
		MYL_NO_DISCARD constexpr vec operator/(const value_type& rhs) const { return vec(x / rhs, y / rhs); }
		MYL_NO_DISCARD constexpr vec operator%(const value_type& rhs) const { return vec(x % rhs, y % rhs); }

		constexpr vec& operator+=(const value_type& rhs) { x += rhs; y += rhs; return *this; }
		constexpr vec& operator-=(const value_type& rhs) { x -= rhs; y -= rhs; return *this; }
		constexpr vec& operator*=(const value_type& rhs) { x *= rhs; y *= rhs; return *this; }
		constexpr vec& operator/=(const value_type& rhs) { x /= rhs; y /= rhs; return *this; }
		constexpr vec& operator%=(const value_type& rhs) { x %= rhs; y %= rhs; return *this; }

		// vec2 operators

		MYL_NO_DISCARD constexpr vec operator+(const vec& rhs) const { return vec(x + rhs.x, y + rhs.y); }
		MYL_NO_DISCARD constexpr vec operator-(const vec& rhs) const { return vec(x - rhs.x, y - rhs.y); }
		MYL_NO_DISCARD constexpr vec operator*(const vec& rhs) const { return vec(x * rhs.x, y * rhs.y); }
		MYL_NO_DISCARD constexpr vec operator/(const vec& rhs) const { return vec(x / rhs.x, y / rhs.y); }
		MYL_NO_DISCARD constexpr vec operator%(const vec& rhs) const { return vec(x % rhs.x, y % rhs.y); }

		constexpr vec& operator+=(const vec& rhs) { x += rhs.x; y += rhs.y; return *this; }
		constexpr vec& operator-=(const vec& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		constexpr vec& operator*=(const vec& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
		constexpr vec& operator/=(const vec& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
		constexpr vec& operator%=(const vec& rhs) { x %= rhs.x; y %= rhs.y; return *this; }
	};

	template<typename T> using vec2 = vec<2, T>;

	using i8vec2 = vec2<i8>;
	using i16vec2 = vec2<i16>;
	using i32vec2 = vec2<i32>;
	using i64vec2 = vec2<i64>;

	using u8vec2 = vec2<u8>;
	using u16vec2 = vec2<u16>;
	using u32vec2 = vec2<u32>;
	using u64vec2 = vec2<u64>;

	using f32vec2 = vec2<f32>;
	using f64vec2 = vec2<f64>;
}
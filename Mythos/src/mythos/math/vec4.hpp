#pragma once
#include "structure_def.hpp"

#include <mythos/defines.hpp>

#include <utility> // std::move

namespace myl {
	template<typename T>
	struct vec<4, T> {
		using value_type = T;

		union {
			value_type data[4];
			struct { value_type x, y, z, w; };
			struct { value_type r, g, b, a; };
			struct { value_type s, t, p, q; };
		};

		// Constructors

		constexpr vec() = default;

		constexpr explicit vec(const value_type& a_scalar)
			: data{ a_scalar, a_scalar, a_scalar, a_scalar } {}

		constexpr vec(const value_type& a_x, const value_type& a_y, const value_type& a_z, const value_type& a_w)
			: data{ a_x, a_y, a_z, a_w } {}

		constexpr vec(value_type&& a_x, value_type&& a_y, value_type&& a_z, value_type&& a_w)
			: data{ std::move(a_x), std::move(a_y), std::move(a_z), std::move(a_w) } {}

		// Vector conversion constructors

		constexpr explicit vec(const vec<2, value_type>& a_vec2a, const vec<2, value_type>& a_vec2b)
			: data{ a_vec2a.x, a_vec2a.y, a_vec2b.x, a_vec2b.y } {}

		constexpr explicit vec(const vec<2, value_type>& a_vec2, const value_type& a_z, const value_type& a_w)
			: data{ a_vec2.x, a_vec2.y, a_z, a_w } {}

		constexpr explicit vec(const value_type& a_x, const vec<2, value_type>& a_vec2, const value_type& a_w)
			: data{ a_x, a_vec2.x, a_vec2.y, a_w } {}

		constexpr explicit vec(const value_type& a_x, const value_type& a_y, const vec<2, value_type>& a_vec2)
			: data{ a_x, a_y, a_vec2.x, a_vec2.y } {}

		constexpr explicit vec(const vec<3, value_type>& a_vec3, const value_type& a_w)
			: data{ a_vec3.x, a_vec3.y, a_vec3.z, a_w } {}

		constexpr explicit vec(const value_type& a_x, const vec<3, value_type>& a_vec3)
			: data{ a_x, a_vec3.x, a_vec3.y, a_vec3.z } {}

		// Utilities

		MYL_NO_DISCARD static constexpr vec zero() { return vec{ 0, 0, 0, 0 }; }
		MYL_NO_DISCARD static constexpr vec one() { return vec{ 1, 1, 1, 1 }; }

		// Access operators

		MYL_NO_DISCARD constexpr value_type& operator[](std::size_t i) { return data[i]; }
		MYL_NO_DISCARD constexpr const value_type& operator[](std::size_t i) const { return data[i]; }

		// Comparison operators

		MYL_NO_DISCARD constexpr bool operator==(const vec& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }

		// Unary operators

		MYL_NO_DISCARD constexpr vec operator-() const { return vec{ -x, -y, -z, -w }; }

		// Scaler operators

		MYL_NO_DISCARD constexpr vec operator+(const value_type& rhs) const { return vec{ x + rhs, y + rhs, z + rhs, w + rhs }; }
		MYL_NO_DISCARD constexpr vec operator-(const value_type& rhs) const { return vec{ x - rhs, y - rhs, z - rhs, w - rhs }; }
		MYL_NO_DISCARD constexpr vec operator*(const value_type& rhs) const { return vec{ x * rhs, y * rhs, z * rhs, w * rhs }; }
		MYL_NO_DISCARD constexpr vec operator/(const value_type& rhs) const { return vec{ x / rhs, y / rhs, z / rhs, w / rhs }; }
		MYL_NO_DISCARD constexpr vec operator%(const value_type& rhs) const { return vec{ x % rhs, y % rhs, z % rhs, w % rhs }; }

		constexpr vec& operator+=(const value_type& rhs) { x += rhs; y += rhs; z += rhs; w += rhs; return *this; }
		constexpr vec& operator-=(const value_type& rhs) { x -= rhs; y -= rhs; z -= rhs; w -= rhs; return *this; }
		constexpr vec& operator*=(const value_type& rhs) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
		constexpr vec& operator/=(const value_type& rhs) { x /= rhs; y /= rhs; z /= rhs; w /= rhs; return *this; }
		constexpr vec& operator%=(const value_type& rhs) { x %= rhs; y %= rhs; z %= rhs; w %= rhs; return *this; }

		// vec2 operators

		MYL_NO_DISCARD constexpr vec operator+(const vec<2, value_type>& rhs) const { return vec{ x + rhs.x, y + rhs.y, z, w }; }
		MYL_NO_DISCARD constexpr vec operator-(const vec<2, value_type>& rhs) const { return vec{ x - rhs.x, y - rhs.y, z, w }; }
		MYL_NO_DISCARD constexpr vec operator*(const vec<2, value_type>& rhs) const { return vec{ x * rhs.x, y * rhs.y, z, w }; }
		MYL_NO_DISCARD constexpr vec operator/(const vec<2, value_type>& rhs) const { return vec{ x / rhs.x, y / rhs.y, z, w }; }
		MYL_NO_DISCARD constexpr vec operator%(const vec<2, value_type>& rhs) const { return vec{ x % rhs.x, y % rhs.y, z, w }; }

		constexpr vec& operator+=(const vec<2, value_type>& rhs) { x += rhs.x; y += rhs.y; return *this; }
		constexpr vec& operator-=(const vec<2, value_type>& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
		constexpr vec& operator*=(const vec<2, value_type>& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
		constexpr vec& operator/=(const vec<2, value_type>& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
		constexpr vec& operator%=(const vec<2, value_type>& rhs) { x %= rhs.x; y %= rhs.y; return *this; }

		// vec3 operators

		MYL_NO_DISCARD constexpr vec operator+(const vec<3, value_type>& rhs) const { return vec{ x + rhs.x, y + rhs.y, z + rhs.z, w }; }
		MYL_NO_DISCARD constexpr vec operator-(const vec<3, value_type>& rhs) const { return vec{ x - rhs.x, y - rhs.y, z - rhs.z, w }; }
		MYL_NO_DISCARD constexpr vec operator*(const vec<3, value_type>& rhs) const { return vec{ x * rhs.x, y * rhs.y, z * rhs.z, w }; }
		MYL_NO_DISCARD constexpr vec operator/(const vec<3, value_type>& rhs) const { return vec{ x / rhs.x, y / rhs.y, z / rhs.z, w }; }
		MYL_NO_DISCARD constexpr vec operator%(const vec<3, value_type>& rhs) const { return vec{ x % rhs.x, y % rhs.y, z % rhs.z, w }; }

		constexpr vec& operator+=(const vec<3, value_type>& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
		constexpr vec& operator-=(const vec<3, value_type>& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
		constexpr vec& operator*=(const vec<3, value_type>& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
		constexpr vec& operator/=(const vec<3, value_type>& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
		constexpr vec& operator%=(const vec<3, value_type>& rhs) { x %= rhs.x; y %= rhs.y; z %= rhs.z; return *this; }

		// vec4 operators

		MYL_NO_DISCARD constexpr vec operator+(const vec& rhs) const { return vec{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w }; }
		MYL_NO_DISCARD constexpr vec operator-(const vec& rhs) const { return vec{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w }; }
		MYL_NO_DISCARD constexpr vec operator*(const vec& rhs) const { return vec{ x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w }; }
		MYL_NO_DISCARD constexpr vec operator/(const vec& rhs) const { return vec{ x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w }; }
		MYL_NO_DISCARD constexpr vec operator%(const vec& rhs) const { return vec{ x % rhs.x, y % rhs.y, z % rhs.z, w % rhs.w }; }

		constexpr vec& operator+=(const vec& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
		constexpr vec& operator-=(const vec& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
		constexpr vec& operator*=(const vec& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
		constexpr vec& operator/=(const vec& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
		constexpr vec& operator%=(const vec& rhs) { x %= rhs.x; y %= rhs.y; z %= rhs.z; w %= rhs.w; return *this; }
	};

	template<typename T> using vec4 = vec<4, T>;

	using i8vec4 = vec4<i8>;
	using i16vec4 = vec4<i16>;
	using i32vec4 = vec4<i32>;
	using i64vec4 = vec4<i64>;

	using u8vec4 = vec4<u8>;
	using u16vec4 = vec4<u16>;
	using u32vec4 = vec4<u32>;
	using u64vec4 = vec4<u64>;

	using f32vec4 = vec4<f32>;
	using f64vec4 = vec4<f64>;
}
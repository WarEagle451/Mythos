#pragma once
#include <mythos/defines.hpp>

#include "structure_def.hpp"

#include <utility> // std::move

namespace myl {
	namespace math {
		template<typename Type>
		struct vec<3, Type> {
			using value_type = Type;
			using type = vec<3, value_type>;

			union {
				value_type data[3];
				struct { value_type x, y, z; };
				struct { value_type r, g, b; };
			};

			// Constructors

			constexpr vec() = default;

			constexpr explicit vec(const value_type& a_scalar)
				: data{ a_scalar, a_scalar, a_scalar } {}

			constexpr vec(const value_type& a_x, const value_type& a_y, const value_type& a_z)
				: data{ a_x, a_y, a_z } {}

			constexpr vec(value_type&& a_x, value_type&& a_y, value_type&& a_z)
				: data{ std::move(a_x), std::move(a_y), std::move(a_z) } {}

			// Vector conversion constructors

			constexpr explicit vec(const vec<2, value_type>& a_vec2, const value_type& a_z)
				: data{ a_vec2.x, a_vec2.y, a_z } {}

			constexpr explicit vec(const value_type& a_x, const vec<2, value_type>& a_vec2)
				: data{ a_x, a_vec2.x, a_vec2.y } {}

			// Access operators

			MYL_NO_DISCARD constexpr value_type& operator[](std::size_t i) { return data[i]; }
			MYL_NO_DISCARD constexpr const value_type& operator[](std::size_t i) const { return data[i]; }

			// Comparison operators

			MYL_NO_DISCARD constexpr bool operator==(const type& rhs) const { return x == rhs.x && y = rhs.y && z == rhs.z; }
			/// spaceship?

			// Scaler operators

			MYL_NO_DISCARD constexpr type operator+(const value_type& rhs) const { return type(x + rhs, y + rhs, z + rhs); }
			MYL_NO_DISCARD constexpr type operator-(const value_type& rhs) const { return type(x - rhs, y - rhs, z - rhs); }
			MYL_NO_DISCARD constexpr type operator*(const value_type& rhs) const { return type(x * rhs, y * rhs, z * rhs); }
			MYL_NO_DISCARD constexpr type operator/(const value_type& rhs) const { return type(x / rhs, y / rhs, z / rhs); }
			MYL_NO_DISCARD constexpr type operator%(const value_type& rhs) const { return type(x % rhs, y % rhs, z % rhs); }

			constexpr type& operator+=(const value_type& rhs) { x += rhs; y += rhs; z += rhs; return *this; }
			constexpr type& operator-=(const value_type& rhs) { x -= rhs; y -= rhs; z -= rhs; return *this; }
			constexpr type& operator*=(const value_type& rhs) { x *= rhs; y *= rhs; z *= rhs; return *this; }
			constexpr type& operator/=(const value_type& rhs) { x /= rhs; y /= rhs; z /= rhs; return *this; }
			constexpr type& operator%=(const value_type& rhs) { x %= rhs; y %= rhs; z %= rhs; return *this; }

			// vec2 operators

			MYL_NO_DISCARD constexpr type operator+(const vec<2, value_type>& rhs) const { return type(x + rhs.x, y + rhs.y, z); }
			MYL_NO_DISCARD constexpr type operator-(const vec<2, value_type>& rhs) const { return type(x - rhs.x, y - rhs.y, z); }
			MYL_NO_DISCARD constexpr type operator*(const vec<2, value_type>& rhs) const { return type(x * rhs.x, y * rhs.y, z); }
			MYL_NO_DISCARD constexpr type operator/(const vec<2, value_type>& rhs) const { return type(x / rhs.x, y / rhs.y, z); }
			MYL_NO_DISCARD constexpr type operator%(const vec<2, value_type>& rhs) const { return type(x % rhs.x, y % rhs.y, z); }

			constexpr type& operator+=(const vec<2, value_type>& rhs) { x += rhs.x; y += rhs.y; return *this; }
			constexpr type& operator-=(const vec<2, value_type>& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
			constexpr type& operator*=(const vec<2, value_type>& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
			constexpr type& operator/=(const vec<2, value_type>& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
			constexpr type& operator%=(const vec<2, value_type>& rhs) { x %= rhs.x; y %= rhs.y; return *this; }

			// vec3 operators

			MYL_NO_DISCARD constexpr type operator+(const type& rhs) const { return type(x + rhs.x, y + rhs.y, z + rhs.z); }
			MYL_NO_DISCARD constexpr type operator-(const type& rhs) const { return type(x - rhs.x, y - rhs.y, z - rhs.z); }
			MYL_NO_DISCARD constexpr type operator*(const type& rhs) const { return type(x * rhs.x, y * rhs.y, z * rhs.z); }
			MYL_NO_DISCARD constexpr type operator/(const type& rhs) const { return type(x / rhs.x, y / rhs.y, z / rhs.z); }
			MYL_NO_DISCARD constexpr type operator%(const type& rhs) const { return type(x % rhs.x, y % rhs.y, z % rhs.z); }

			constexpr type& operator+=(const type& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
			constexpr type& operator-=(const type& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
			constexpr type& operator*=(const type& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
			constexpr type& operator/=(const type& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
			constexpr type& operator%=(const type& rhs) { x %= rhs.x; y %= rhs.y; z %= rhs.z; return *this; }
		};
	}

	template<typename Type> using vec3 = math::vec<3, Type>;

	using i8vec3 = vec3<i8>;
	using i16vec3 = vec3<i16>;
	using i32vec3 = vec3<i32>;
	using i64vec3 = vec3<i64>;

	using u8vec3 = vec3<u8>;
	using u16vec3 = vec3<u16>;
	using u32vec3 = vec3<u32>;
	using u64vec3 = vec3<u64>;

	using f32vec3 = vec3<f32>;
	using f64vec3 = vec3<f64>;
}
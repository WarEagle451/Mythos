#pragma once
#include <mythos/defines.hpp>

#include "structure_def.hpp"

#include <utility> // std::move

namespace myl {
	namespace math {
		template<typename Type>
		struct vec<4, Type> {
			using value_type = Type;
			using type = vec<4, value_type>;

			union {
				value_type data[4];
				struct { value_type x, y, z, w; };
				struct { value_type r, g, b, a; };
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

			// Access operators

			MYL_NO_DISCARD constexpr value_type& operator[](std::size_t i) { return data[i]; }
			MYL_NO_DISCARD constexpr const value_type& operator[](std::size_t i) const { return data[i]; }

			// Comparison operators

			MYL_NO_DISCARD constexpr bool operator==(const type& rhs) const { return x == rhs.x && y = rhs.y && z == rhs.z && w == rhs.w; }
			/// spaceship?

			// Scaler operators

			MYL_NO_DISCARD constexpr type operator+(const value_type& rhs) const { return type(x + rhs, y + rhs, z + rhs, w + rhs); }
			MYL_NO_DISCARD constexpr type operator-(const value_type& rhs) const { return type(x - rhs, y - rhs, z - rhs, w - rhs); }
			MYL_NO_DISCARD constexpr type operator*(const value_type& rhs) const { return type(x * rhs, y * rhs, z * rhs, w * rhs); }
			MYL_NO_DISCARD constexpr type operator/(const value_type& rhs) const { return type(x / rhs, y / rhs, z / rhs, w / rhs); }
			MYL_NO_DISCARD constexpr type operator%(const value_type& rhs) const { return type(x % rhs, y % rhs, z % rhs, w % rhs); }

			constexpr type& operator+=(const value_type& rhs) { x += rhs; y += rhs; z += rhs; w += rhs; return *this; }
			constexpr type& operator-=(const value_type& rhs) { x -= rhs; y -= rhs; z -= rhs; w -= rhs; return *this; }
			constexpr type& operator*=(const value_type& rhs) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
			constexpr type& operator/=(const value_type& rhs) { x /= rhs; y /= rhs; z /= rhs; w /= rhs; return *this; }
			constexpr type& operator%=(const value_type& rhs) { x %= rhs; y %= rhs; z %= rhs; w %= rhs; return *this; }

			// vec2 operators

			MYL_NO_DISCARD constexpr type operator+(const vec<2, value_type>& rhs) const { return type(x + rhs.x, y + rhs.y, z, w); }
			MYL_NO_DISCARD constexpr type operator-(const vec<2, value_type>& rhs) const { return type(x - rhs.x, y - rhs.y, z, w); }
			MYL_NO_DISCARD constexpr type operator*(const vec<2, value_type>& rhs) const { return type(x * rhs.x, y * rhs.y, z, w); }
			MYL_NO_DISCARD constexpr type operator/(const vec<2, value_type>& rhs) const { return type(x / rhs.x, y / rhs.y, z, w); }
			MYL_NO_DISCARD constexpr type operator%(const vec<2, value_type>& rhs) const { return type(x % rhs.x, y % rhs.y, z, w); }

			constexpr type& operator+=(const vec<2, value_type>& rhs) { x += rhs.x; y += rhs.y; return *this; }
			constexpr type& operator-=(const vec<2, value_type>& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
			constexpr type& operator*=(const vec<2, value_type>& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
			constexpr type& operator/=(const vec<2, value_type>& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
			constexpr type& operator%=(const vec<2, value_type>& rhs) { x %= rhs.x; y %= rhs.y; return *this; }

			// vec3 operators

			MYL_NO_DISCARD constexpr type operator+(const vec<3, value_type>& rhs) const { return type(x + rhs.x, y + rhs.y, z + rhs.z, w); }
			MYL_NO_DISCARD constexpr type operator-(const vec<3, value_type>& rhs) const { return type(x - rhs.x, y - rhs.y, z - rhs.z, w); }
			MYL_NO_DISCARD constexpr type operator*(const vec<3, value_type>& rhs) const { return type(x * rhs.x, y * rhs.y, z * rhs.z, w); }
			MYL_NO_DISCARD constexpr type operator/(const vec<3, value_type>& rhs) const { return type(x / rhs.x, y / rhs.y, z / rhs.z, w); }
			MYL_NO_DISCARD constexpr type operator%(const vec<3, value_type>& rhs) const { return type(x % rhs.x, y % rhs.y, z % rhs.z, w); }

			constexpr type& operator+=(const vec<3, value_type>& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
			constexpr type& operator-=(const vec<3, value_type>& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
			constexpr type& operator*=(const vec<3, value_type>& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
			constexpr type& operator/=(const vec<3, value_type>& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
			constexpr type& operator%=(const vec<3, value_type>& rhs) { x %= rhs.x; y %= rhs.y; z %= rhs.z; return *this; }

			// vec4 operators

			MYL_NO_DISCARD constexpr type operator+(const type& rhs) const { return type(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w); }
			MYL_NO_DISCARD constexpr type operator-(const type& rhs) const { return type(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w); }
			MYL_NO_DISCARD constexpr type operator*(const type& rhs) const { return type(x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w); }
			MYL_NO_DISCARD constexpr type operator/(const type& rhs) const { return type(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w); }
			MYL_NO_DISCARD constexpr type operator%(const type& rhs) const { return type(x % rhs.x, y % rhs.y, z % rhs.z, w % rhs.w); }

			constexpr type& operator+=(const type& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
			constexpr type& operator-=(const type& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
			constexpr type& operator*=(const type& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
			constexpr type& operator/=(const type& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
			constexpr type& operator%=(const type& rhs) { x %= rhs.x; y %= rhs.y; z %= rhs.z; w %= rhs.w; return *this; }
		};
	}

	template<typename Type> using vec4 = math::vec<4, Type>;
	using ivec4 = math::vec<4, int>;
	using fvec4 = math::vec<4, float>;
	using dvec4 = math::vec<4, double>;
}
#pragma once
#include <mythos/defines.hpp>

#include "structure_def.hpp"

#include <utility> // std::move

namespace myl {
	namespace math {
		template<typename Type>
		struct vec<2, Type> {
			using value_type = Type;
			using type = vec<2, value_type>;

			union {
				value_type data[2];
				struct { value_type x, y; };
				struct { value_type r, g; };
			};

			// Constructors

			constexpr vec() = default;

			constexpr explicit vec(const value_type& a_scalar)
				: data{ a_scalar, a_scalar } {}

			constexpr vec(const value_type& a_x, const value_type& a_y)
				: data{ a_x, a_y } {}

			constexpr vec(value_type&& a_x, value_type&& a_y)
				: data{ std::move(a_x), std::move(a_y) } {}

			// Access operators

			MYL_NO_DISCARD constexpr value_type& operator[](std::size_t i) { return data[i]; }
			MYL_NO_DISCARD constexpr const value_type& operator[](std::size_t i) const { return data[i]; }

			// Comparison operators

			MYL_NO_DISCARD constexpr bool operator==(const type& rhs) const { return x == rhs.x && y = rhs.y; }
			/// spaceship?

			// Scaler operators

			MYL_NO_DISCARD constexpr type operator+(const value_type& rhs) const { return type(x + rhs, y + rhs); }
			MYL_NO_DISCARD constexpr type operator-(const value_type& rhs) const { return type(x - rhs, y - rhs); }
			MYL_NO_DISCARD constexpr type operator*(const value_type& rhs) const { return type(x * rhs, y * rhs); }
			MYL_NO_DISCARD constexpr type operator/(const value_type& rhs) const { return type(x / rhs, y / rhs); }
			MYL_NO_DISCARD constexpr type operator%(const value_type& rhs) const { return type(x % rhs, y % rhs); }

			constexpr type& operator+=(const value_type& rhs) { x += rhs; y += rhs; return *this; }
			constexpr type& operator-=(const value_type& rhs) { x -= rhs; y -= rhs; return *this; }
			constexpr type& operator*=(const value_type& rhs) { x *= rhs; y *= rhs; return *this; }
			constexpr type& operator/=(const value_type& rhs) { x /= rhs; y /= rhs; return *this; }
			constexpr type& operator%=(const value_type& rhs) { x %= rhs; y %= rhs; return *this; }

			// vec2 operators

			MYL_NO_DISCARD constexpr type operator+(const type& rhs) const { return type(x + rhs.x, y + rhs.y); }
			MYL_NO_DISCARD constexpr type operator-(const type& rhs) const { return type(x - rhs.x, y - rhs.y); }
			MYL_NO_DISCARD constexpr type operator*(const type& rhs) const { return type(x * rhs.x, y * rhs.y); }
			MYL_NO_DISCARD constexpr type operator/(const type& rhs) const { return type(x / rhs.x, y / rhs.y); }
			MYL_NO_DISCARD constexpr type operator%(const type& rhs) const { return type(x % rhs.x, y % rhs.y); }

			constexpr type& operator+=(const type& rhs) { x += rhs.x; y += rhs.y; return *this; }
			constexpr type& operator-=(const type& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
			constexpr type& operator*=(const type& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
			constexpr type& operator/=(const type& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
			constexpr type& operator%=(const type& rhs) { x %= rhs.x; y %= rhs.y; return *this; }
		};
	}

	template<typename Type> using vec2 = math::vec<2, Type>;
	using ivec2 = math::vec<2, int>;
	using fvec2 = math::vec<2, float>;
	using dvec2 = math::vec<2, double>;
}
#pragma once
#include "vec2.hpp"
#include "vec3.hpp"

namespace myl {
	namespace math {
		template<typename Type>
		struct mat<3, 2, Type> {
			using value_type = Type;
			using type = mat<3, 2, value_type>;
			using transpose_type = mat<2, 3, value_type>;
			using col_type = vec<3, value_type>;
			using row_type = vec<2, value_type>;
		private:
			col_type data[2];
		public:
			// Constructors

			constexpr mat() = default;

			constexpr explicit mat(const value_type& a_scalar)
				: data{ col_type(a_scalar), col_type(a_scalar) } {}

			constexpr mat(const value_type& x0, const value_type& y0, const value_type& z0, const value_type& x1, const value_type& y1, const value_type& z1)
				: data{ col_type(x0, y0, z0), col_type(x1, y1, z1) } {}

			constexpr mat(value_type&& x0, value_type&& y0, value_type&& z0, value_type&& x1, value_type&& y1, value_type&& z1)
				: data{ col_type(std::move(x0), std::move(y0), std::move(z0)), col_type(std::move(x1), std::move(y1), std::move(z1)) } {}

			constexpr mat(const col_type& col0, const col_type& col1)
				: data{ col0, col1 } {}

			constexpr mat(col_type&& col0, col_type&& col1)
				: data{ std::move(col0), std::move(col1) } {}

			// Access operators

			MYL_NO_DISCARD constexpr col_type& operator[](std::size_t i) { return data[i]; }
			MYL_NO_DISCARD constexpr const col_type& operator[](std::size_t i) const { return data[i]; }

			// Comparison operators

			MYL_NO_DISCARD constexpr bool operator==(const type& rhs) const { return data[0] == rhs[0] && data[1] == rhs[1]; }

			/// spaceship

			// Scaler operators

			MYL_NO_DISCARD constexpr type operator+(const value_type& rhs) const { return type(data[0] + rhs, data[1] + rhs); }
			MYL_NO_DISCARD constexpr type operator-(const value_type& rhs) const { return type(data[0] - rhs, data[1] - rhs); }
			MYL_NO_DISCARD constexpr type operator*(const value_type& rhs) const { return type(data[0] * rhs, data[1] * rhs); }
			MYL_NO_DISCARD constexpr type operator/(const value_type& rhs) const { return type(data[0] / rhs, data[1] / rhs); }
			MYL_NO_DISCARD constexpr type operator%(const value_type& rhs) const { return type(data[0] % rhs, data[1] % rhs); }

			constexpr type& operator+=(const value_type& rhs) { data[0] += rhs; data[1] += rhs; return *this; }
			constexpr type& operator-=(const value_type& rhs) { data[0] -= rhs; data[1] -= rhs; return *this; }
			constexpr type& operator*=(const value_type& rhs) { data[0] *= rhs; data[1] *= rhs; return *this; }
			constexpr type& operator/=(const value_type& rhs) { data[0] /= rhs; data[1] /= rhs; return *this; }
			constexpr type& operator%=(const value_type& rhs) { data[0] %= rhs; data[1] %= rhs; return *this; }

			// mat3x2 operators

			MYL_NO_DISCARD constexpr type operator+(const type& rhs) const { return type(data[0] + rhs[0], data[1] + rhs[1]); }
			MYL_NO_DISCARD constexpr type operator-(const type& rhs) const { return type(data[0] - rhs[0], data[1] - rhs[1]); }

			constexpr type& operator+=(const type& rhs) { data[0] += rhs[0]; data[1] += rhs[1]; return *this; }
			constexpr type& operator-=(const type& rhs) { data[0] -= rhs[0]; data[1] -= rhs[1]; return *this; }
		};
	}

	template<typename Type> using mat3x2 = math::mat<3, 2, Type>;
	using imat3x2 = math::mat<3, 2, int>;
	using fmat3x2 = math::mat<3, 2, float>;
	using dmat3x2 = math::mat<3, 2, double>;
}
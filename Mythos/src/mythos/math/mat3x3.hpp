#pragma once
#include "vec3.hpp"

namespace myl {
	namespace math {
		template<typename Type>
		struct mat<3, 3, Type> {
			using value_type = Type;
			using type = mat<3, 3, value_type>;
			using transpose_type = mat<3, 3, value_type>;
			using col_type = vec<3, value_type>;
			using row_type = vec<3, value_type>;
		private:
			col_type data[3];
		public:
			// Constructors

			constexpr mat() = default;

			constexpr explicit mat(const value_type& a_scalar)
				: data{ col_type(a_scalar), col_type(a_scalar), col_type(a_scalar) } {}

			constexpr mat(const value_type& x0, const value_type& y0, const value_type& z0, const value_type& x1, const value_type& y1, const value_type& z1, const value_type& x2, const value_type& y2, const value_type& z2)
				: data{ col_type(x0, y0, z0), col_type(x1, y1, z1), col_type(x2, y2, z2) } {}

			constexpr mat(value_type&& x0, value_type&& y0, value_type&& z0, value_type&& x1, value_type&& y1, value_type&& z1, value_type&& x2, value_type&& y2, value_type&& z2)
				: data{ col_type(std::move(x0), std::move(y0), std::move(z0)), col_type(std::move(x1), std::move(y1), std::move(z1)), col_type(std::move(x2), std::move(y2), std::move(z2)) } {}

			constexpr mat(const col_type& col0, const col_type& col1, const col_type& col2)
				: data{ col0, col1, col2 } {}

			constexpr mat(col_type&& col0, col_type&& col1, col_type&& col2)
				: data{ std::move(col0), std::move(col1), std::move(col2) } {}

			// Access operators

			MYL_NO_DISCARD constexpr col_type& operator[](std::size_t i) { return data[i]; }
			MYL_NO_DISCARD constexpr const col_type& operator[](std::size_t i) const { return data[i]; }

			// Comparison operators

			MYL_NO_DISCARD constexpr bool operator==(const type& rhs) const { return data[0] == rhs[0] && data[1] == rhs[1] && data[2] == rhs[2]; }

			/// spaceship

			// Scaler operators

			MYL_NO_DISCARD constexpr type operator+(const value_type& rhs) const { return type(data[0] + rhs, data[1] + rhs, data[2] + rhs); }
			MYL_NO_DISCARD constexpr type operator-(const value_type& rhs) const { return type(data[0] - rhs, data[1] - rhs, data[2] - rhs); }
			MYL_NO_DISCARD constexpr type operator*(const value_type& rhs) const { return type(data[0] * rhs, data[1] * rhs, data[2] * rhs); }
			MYL_NO_DISCARD constexpr type operator/(const value_type& rhs) const { return type(data[0] / rhs, data[1] / rhs, data[2] / rhs); }
			MYL_NO_DISCARD constexpr type operator%(const value_type& rhs) const { return type(data[0] % rhs, data[1] % rhs, data[2] % rhs); }

			constexpr type& operator+=(const value_type& rhs) { data[0] += rhs; data[1] += rhs; data[2] += rhs; return *this; }
			constexpr type& operator-=(const value_type& rhs) { data[0] -= rhs; data[1] -= rhs; data[2] -= rhs; return *this; }
			constexpr type& operator*=(const value_type& rhs) { data[0] *= rhs; data[1] *= rhs; data[2] *= rhs; return *this; }
			constexpr type& operator/=(const value_type& rhs) { data[0] /= rhs; data[1] /= rhs; data[2] /= rhs; return *this; }
			constexpr type& operator%=(const value_type& rhs) { data[0] %= rhs; data[1] %= rhs; data[2] %= rhs; return *this; }

			// mat3x3 operators

			MYL_NO_DISCARD constexpr type operator+(const type& rhs) const { return type(data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2]); }
			MYL_NO_DISCARD constexpr type operator-(const type& rhs) const { return type(data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2]); }

			MYL_NO_DISCARD constexpr type operator*(const type& rhs) const {
				return type(
					data[0][0] * rhs[0][0] + data[1][0] * rhs[0][1] + data[2][0] * rhs[0][2], // x0
					data[0][1] * rhs[0][0] + data[1][1] * rhs[0][1] + data[2][1] * rhs[0][2], // y0
					data[0][2] * rhs[0][0] + data[1][2] * rhs[0][1] + data[2][2] * rhs[0][2], // z0
					data[0][0] * rhs[1][0] + data[1][0] * rhs[1][1] + data[2][0] * rhs[1][2], // x1
					data[0][1] * rhs[1][0] + data[1][1] * rhs[1][1] + data[2][1] * rhs[1][2], // y1
					data[0][2] * rhs[1][0] + data[1][2] * rhs[1][1] + data[2][2] * rhs[1][2], // z1
					data[0][0] * rhs[2][0] + data[1][0] * rhs[2][1] + data[2][0] * rhs[2][2], // x2
					data[0][1] * rhs[2][0] + data[1][1] * rhs[2][1] + data[2][1] * rhs[2][2], // y2
					data[0][2] * rhs[2][0] + data[1][2] * rhs[2][1] + data[2][2] * rhs[2][2]);// z2
			}

			MYL_NO_DISCARD constexpr type operator/(const type& rhs) const { return *this * inverse(rhs); }

			constexpr type& operator+=(const type& rhs) { data[0] += rhs[0]; data[1] += rhs[1]; data[2] += rhs[2]; return *this; }
			constexpr type& operator-=(const type& rhs) { data[0] -= rhs[0]; data[1] -= rhs[1]; data[2] -= rhs[2]; return *this; }
			constexpr type& operator*=(const type& rhs) { return *this = (*this * rhs); }
			constexpr type& operator/=(const type& rhs) { return *this *= inverse(rhs); }
		};
	}

	template<typename Type> using mat3x3 = math::mat<3, 3, Type>;
	using imat3x3 = math::mat<3, 3, int>;
	using fmat3x3 = math::mat<3, 3, float>;
	using dmat3x3 = math::mat<3, 3, double>;
}
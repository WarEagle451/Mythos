#pragma once
#include "vec4.hpp"

namespace myl {
	namespace math {
		template<typename Type>
		struct mat<4, 4, Type> {
			using value_type = Type;
			using type = mat<4, 4, value_type>;
			using transpose_type = mat<4, 4, value_type>;
			using col_type = vec<4, value_type>;
			using row_type = vec<4, value_type>;
		private:
			col_type data[4];
		public:
			// Constructors

			constexpr mat() = default;

			constexpr explicit mat(const value_type& a_scalar)
				: data{ col_type(a_scalar), col_type(a_scalar), col_type(a_scalar), col_type(a_scalar) } {}

			constexpr mat(const value_type& x0, const value_type& y0, const value_type& z0, const value_type& w0, const value_type& x1, const value_type& y1, const value_type& z1, const value_type& w1, const value_type& x2, const value_type& y2, const value_type& z2, const value_type& w2, const value_type& x3, const value_type& y3, const value_type& z3, const value_type& w3)
				: data{ col_type(x0, y0, z0, w0), col_type(x1, y1, z1, w1), col_type(x2, y2, z2, w2), col_type(x3, y3, z3, w3) } {}

			constexpr mat(value_type&& x0, value_type&& y0, value_type&& z0, value_type&& w0, value_type&& x1, value_type&& y1, value_type&& z1, value_type&& w1, value_type&& x2, value_type&& y2, value_type&& z2, value_type&& w2, value_type&& x3, value_type&& y3, value_type&& z3, value_type&& w3)
				: data{ col_type(std::move(x0), std::move(y0), std::move(z0), std::move(w0)), col_type(std::move(x1), std::move(y1), std::move(z1), std::move(w1)), col_type(std::move(x2), std::move(y2), std::move(z2), std::move(w2)), col_type(std::move(x3), std::move(y3), std::move(z3), std::move(w3)) } {}

			constexpr mat(const col_type& col0, const col_type& col1, const col_type& col2, const col_type& col3)
				: data{ col0, col1, col2, col3 } {}

			constexpr mat(col_type&& col0, col_type&& col1, col_type&& col2, col_type&& col3)
				: data{ std::move(col0), std::move(col1), std::move(col2), std::move(col3) } {}

			// Access operators

			MYL_NO_DISCARD constexpr col_type& operator[](std::size_t i) { return data[i]; }
			MYL_NO_DISCARD constexpr const col_type& operator[](std::size_t i) const { return data[i]; }

			// Comparison operators

			MYL_NO_DISCARD constexpr bool operator==(const type& rhs) const { return data[0] == rhs[0] && data[1] == rhs[1] && data[2] == rhs[2] && data[3] == rhs[3]; }

			/// spaceship

			// Scaler operators

			MYL_NO_DISCARD constexpr type operator+(const value_type& rhs) const { return type(data[0] + rhs, data[1] + rhs, data[2] + rhs, data[3] + rhs); }
			MYL_NO_DISCARD constexpr type operator-(const value_type& rhs) const { return type(data[0] - rhs, data[1] - rhs, data[2] - rhs, data[3] - rhs); }
			MYL_NO_DISCARD constexpr type operator*(const value_type& rhs) const { return type(data[0] * rhs, data[1] * rhs, data[2] * rhs, data[3] * rhs); }
			MYL_NO_DISCARD constexpr type operator/(const value_type& rhs) const { return type(data[0] / rhs, data[1] / rhs, data[2] / rhs, data[3] / rhs); }
			MYL_NO_DISCARD constexpr type operator%(const value_type& rhs) const { return type(data[0] % rhs, data[1] % rhs, data[2] % rhs, data[3] % rhs); }

			constexpr type& operator+=(const value_type& rhs) { data[0] += rhs; data[1] += rhs; data[2] += rhs; data[3] += rhs; return *this; }
			constexpr type& operator-=(const value_type& rhs) { data[0] -= rhs; data[1] -= rhs; data[2] -= rhs; data[3] -= rhs; return *this; }
			constexpr type& operator*=(const value_type& rhs) { data[0] *= rhs; data[1] *= rhs; data[2] *= rhs; data[3] *= rhs; return *this; }
			constexpr type& operator/=(const value_type& rhs) { data[0] /= rhs; data[1] /= rhs; data[2] /= rhs; data[3] /= rhs; return *this; }
			constexpr type& operator%=(const value_type& rhs) { data[0] %= rhs; data[1] %= rhs; data[2] %= rhs; data[3] %= rhs; return *this; }

			// mat4x4 operators

			MYL_NO_DISCARD constexpr type operator+(const type& rhs) const { return type(data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2], data[3] + rhs[3]); }
			MYL_NO_DISCARD constexpr type operator-(const type& rhs) const { return type(data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2], data[3] - rhs[3]); }

			MYL_NO_DISCARD constexpr type operator*(const type& rhs) const {
				return type(
					data[0][0] * rhs[0][0] + data[1][0] * rhs[0][1] + data[2][0] * rhs[0][2] + data[3][0] * rhs[0][3], // x0
					data[0][1] * rhs[0][0] + data[1][1] * rhs[0][1] + data[2][1] * rhs[0][2] + data[3][1] * rhs[0][3], // y0
					data[0][2] * rhs[0][0] + data[1][2] * rhs[0][1] + data[2][2] * rhs[0][2] + data[3][2] * rhs[0][3], // z0
					data[0][3] * rhs[0][0] + data[1][3] * rhs[0][1] + data[2][3] * rhs[0][2] + data[3][3] * rhs[0][3], // w0
					data[0][0] * rhs[1][0] + data[1][0] * rhs[1][1] + data[2][0] * rhs[1][2] + data[3][0] * rhs[1][3], // x1
					data[0][1] * rhs[1][0] + data[1][1] * rhs[1][1] + data[2][1] * rhs[1][2] + data[3][1] * rhs[1][3], // y1
					data[0][2] * rhs[1][0] + data[1][2] * rhs[1][1] + data[2][2] * rhs[1][2] + data[3][2] * rhs[1][3], // z1
					data[0][3] * rhs[1][0] + data[1][3] * rhs[1][1] + data[2][3] * rhs[1][2] + data[3][3] * rhs[1][3], // w1
					data[0][0] * rhs[2][0] + data[1][0] * rhs[2][1] + data[2][0] * rhs[2][2] + data[3][0] * rhs[2][3], // x2
					data[0][1] * rhs[2][0] + data[1][1] * rhs[2][1] + data[2][1] * rhs[2][2] + data[3][1] * rhs[2][3], // y2
					data[0][2] * rhs[2][0] + data[1][2] * rhs[2][1] + data[2][2] * rhs[2][2] + data[3][2] * rhs[2][3], // z2
					data[0][3] * rhs[2][0] + data[1][3] * rhs[2][1] + data[2][3] * rhs[2][2] + data[3][3] * rhs[2][3], // w2
					data[0][0] * rhs[3][0] + data[1][0] * rhs[3][1] + data[2][0] * rhs[3][2] + data[3][0] * rhs[3][3], // x3
					data[0][1] * rhs[3][0] + data[1][1] * rhs[3][1] + data[2][1] * rhs[3][2] + data[3][1] * rhs[3][3], // y3
					data[0][2] * rhs[3][0] + data[1][2] * rhs[3][1] + data[2][2] * rhs[3][2] + data[3][2] * rhs[3][3], // z3
					data[0][3] * rhs[3][0] + data[1][3] * rhs[3][1] + data[2][3] * rhs[3][2] + data[3][3] * rhs[3][3]);// w3
			}

			MYL_NO_DISCARD constexpr type operator/(const type& rhs) const { return *this * inverse(rhs); }

			constexpr type& operator+=(const type& rhs) { data[0] += rhs[0]; data[1] += rhs[1]; data[2] += rhs[2]; data[3] += rhs[3]; return *this; }
			constexpr type& operator-=(const type& rhs) { data[0] -= rhs[0]; data[1] -= rhs[1]; data[2] -= rhs[2]; data[3] -= rhs[3]; return *this; }
			constexpr type& operator*=(const type& rhs) { return *this = (*this * rhs); }
			constexpr type& operator/=(const type& rhs) { return *this *= inverse(rhs); }
		};
	}

	template<typename Type> using mat4x4 = math::mat<4, 4, Type>;

	using i8mat4x4 = mat4x4<i8>;
	using i16mat4x4 = mat4x4<i16>;
	using i32mat4x4 = mat4x4<i32>;
	using i64mat4x4 = mat4x4<i64>;

	using u8mat4x4 = mat4x4<u8>;
	using u16mat4x4 = mat4x4<u16>;
	using u32mat4x4 = mat4x4<u32>;
	using u64mat4x4 = mat4x4<u64>;

	using f32mat4x4 = mat4x4<f32>;
	using f64mat4x4 = mat4x4<f64>;
}
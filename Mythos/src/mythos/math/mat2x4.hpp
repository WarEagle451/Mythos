#pragma once
#include "vec2.hpp"
#include "vec4.hpp"

namespace myl {
	namespace math {
		template<typename Type>
		struct mat<2, 4, Type> {
			using value_type = Type;
			using type = mat<2, 4, value_type>;
			using transpose_type = mat<4, 2, value_type>;
			using col_type = vec<2, value_type>;
			using row_type = vec<4, value_type>;
		private:
			col_type data[4];
		public:
			// Constructors

			constexpr mat() = default;

			constexpr explicit mat(const value_type& a_scalar)
				: data{ col_type(a_scalar), col_type(a_scalar), col_type(a_scalar), col_type(a_scalar) } {}

			constexpr mat(const value_type& x0, const value_type& y0, const value_type& x1, const value_type& y1, const value_type& x2, const value_type& y2, const value_type& x3, const value_type& y3)
				: data{ col_type(x0, y0), col_type(x1, y1), col_type(x2, y2), col_type(x3, y3) } {}

			constexpr mat(value_type&& x0, value_type&& y0, value_type&& x1, value_type&& y1, value_type&& x2, value_type&& y2, value_type&& x3, value_type&& y3)
				: data{ col_type(std::move(x0), std::move(y0)), col_type(std::move(x1), std::move(y1)), col_type(std::move(x2), std::move(y2)), col_type(std::move(x3), std::move(y3)) } {}

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

			// mat2x4 operators

			MYL_NO_DISCARD constexpr type operator+(const type& rhs) const { return type(data[0] + rhs[0], data[1] + rhs[1], data[2] + rhs[2], data[3] + rhs[3]); }
			MYL_NO_DISCARD constexpr type operator-(const type& rhs) const { return type(data[0] - rhs[0], data[1] - rhs[1], data[2] - rhs[2], data[3] - rhs[3]); }

			constexpr type& operator+=(const type& rhs) { data[0] += rhs[0]; data[1] += rhs[1]; data[2] += rhs[2]; data[3] += rhs[3]; return *this; }
			constexpr type& operator-=(const type& rhs) { data[0] -= rhs[0]; data[1] -= rhs[1]; data[2] -= rhs[2]; data[3] -= rhs[3]; return *this; }
		};
	}

	template<typename Type> using mat2x4 = math::mat<2, 4, Type>;

	using i8mat2x4 = mat2x4<i8>;
	using i16mat2x4 = mat2x4<i16>;
	using i32mat2x4 = mat2x4<i32>;
	using i64mat2x4 = mat2x4<i64>;

	using u8mat2x4 = mat2x4<u8>;
	using u16mat2x4 = mat2x4<u16>;
	using u32mat2x4 = mat2x4<u32>;
	using u64mat2x4 = mat2x4<u64>;

	using f32mat2x4 = mat2x4<f32>;
	using f64mat2x4 = mat2x4<f64>;
}
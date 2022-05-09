#pragma once
#include "vec2.hpp"
#include "vec4.hpp"

namespace myl {
	template<typename T>
	struct mat<2, 4, T> {
		using value_type = T;
		using transpose_type = mat<4, 2, value_type>;
		using col_type = vec<2, value_type>;
		using row_type = vec<4, value_type>;
	private:
		col_type m_data[4];
	public:
		// Constructors

		constexpr mat() = default;

		constexpr explicit mat(const value_type& a_scalar)
			: m_data{ col_type(a_scalar), col_type(a_scalar), col_type(a_scalar), col_type(a_scalar) } {}

		constexpr mat(const value_type& x0, const value_type& y0, const value_type& x1, const value_type& y1, const value_type& x2, const value_type& y2, const value_type& x3, const value_type& y3)
			: m_data{ col_type(x0, y0), col_type(x1, y1), col_type(x2, y2), col_type(x3, y3) } {}

		constexpr mat(value_type&& x0, value_type&& y0, value_type&& x1, value_type&& y1, value_type&& x2, value_type&& y2, value_type&& x3, value_type&& y3)
			: m_data{ col_type(std::move(x0), std::move(y0)), col_type(std::move(x1), std::move(y1)), col_type(std::move(x2), std::move(y2)), col_type(std::move(x3), std::move(y3)) } {}

		constexpr mat(const col_type& col0, const col_type& col1, const col_type& col2, const col_type& col3)
			: m_data{ col0, col1, col2, col3 } {}

		constexpr mat(col_type&& col0, col_type&& col1, col_type&& col2, col_type&& col3)
			: m_data{ std::move(col0), std::move(col1), std::move(col2), std::move(col3) } {}

		// Access operators

		MYL_NO_DISCARD constexpr col_type& operator[](std::size_t i) { return m_data[i]; }
		MYL_NO_DISCARD constexpr const col_type& operator[](std::size_t i) const { return m_data[i]; }

		// Comparison operators

		MYL_NO_DISCARD constexpr bool operator==(const mat& rhs) const { return m_data[0] == rhs[0] && m_data[1] == rhs[1] && m_data[2] == rhs[2] && m_data[3] == rhs[3]; }

		/// Spaceship

		// Unary operators

		MYL_NO_DISCARD constexpr mat operator-() const { return mat{ -m_data[0], -m_data[1], -m_data[2], -m_data[3] }; }

		// Scaler operators

		MYL_NO_DISCARD constexpr mat operator+(const value_type& rhs) const { return mat(m_data[0] + rhs, m_data[1] + rhs, m_data[2] + rhs, m_data[3] + rhs); }
		MYL_NO_DISCARD constexpr mat operator-(const value_type& rhs) const { return mat(m_data[0] - rhs, m_data[1] - rhs, m_data[2] - rhs, m_data[3] - rhs); }
		MYL_NO_DISCARD constexpr mat operator*(const value_type& rhs) const { return mat(m_data[0] * rhs, m_data[1] * rhs, m_data[2] * rhs, m_data[3] * rhs); }
		MYL_NO_DISCARD constexpr mat operator/(const value_type& rhs) const { return mat(m_data[0] / rhs, m_data[1] / rhs, m_data[2] / rhs, m_data[3] / rhs); }
		MYL_NO_DISCARD constexpr mat operator%(const value_type& rhs) const { return mat(m_data[0] % rhs, m_data[1] % rhs, m_data[2] % rhs, m_data[3] % rhs); }

		constexpr mat& operator+=(const value_type& rhs) { m_data[0] += rhs; m_data[1] += rhs; m_data[2] += rhs; m_data[3] += rhs; return *this; }
		constexpr mat& operator-=(const value_type& rhs) { m_data[0] -= rhs; m_data[1] -= rhs; m_data[2] -= rhs; m_data[3] -= rhs; return *this; }
		constexpr mat& operator*=(const value_type& rhs) { m_data[0] *= rhs; m_data[1] *= rhs; m_data[2] *= rhs; m_data[3] *= rhs; return *this; }
		constexpr mat& operator/=(const value_type& rhs) { m_data[0] /= rhs; m_data[1] /= rhs; m_data[2] /= rhs; m_data[3] /= rhs; return *this; }
		constexpr mat& operator%=(const value_type& rhs) { m_data[0] %= rhs; m_data[1] %= rhs; m_data[2] %= rhs; m_data[3] %= rhs; return *this; }

		// mat2x4 operators

		MYL_NO_DISCARD constexpr mat operator+(const mat& rhs) const { return mat(m_data[0] + rhs[0], m_data[1] + rhs[1], m_data[2] + rhs[2], m_data[3] + rhs[3]); }
		MYL_NO_DISCARD constexpr mat operator-(const mat& rhs) const { return mat(m_data[0] - rhs[0], m_data[1] - rhs[1], m_data[2] - rhs[2], m_data[3] - rhs[3]); }

		constexpr mat& operator+=(const mat& rhs) { m_data[0] += rhs[0]; m_data[1] += rhs[1]; m_data[2] += rhs[2]; m_data[3] += rhs[3]; return *this; }
		constexpr mat& operator-=(const mat& rhs) { m_data[0] -= rhs[0]; m_data[1] -= rhs[1]; m_data[2] -= rhs[2]; m_data[3] -= rhs[3]; return *this; }
	};

	template<typename T> using mat2x4 = mat<2, 4, T>;

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
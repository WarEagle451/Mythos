#pragma once
#include "vec3.hpp"
#include "vec4.hpp"

namespace myl {
	template<typename T>
	struct mat<4, 3, T> {
		using value_type = T;
		using transpose_type = mat<3, 4, value_type>;
		using col_type = vec<4, value_type>;
		using row_type = vec<3, value_type>;
	private:
		col_type m_data[3];
	public:
		// Constructors

		constexpr mat() = default;

		constexpr explicit mat(const value_type& a_scalar)
			: m_data{ col_type(a_scalar), col_type(a_scalar), col_type(a_scalar) } {}

		constexpr mat(const value_type& x0, const value_type& y0, const value_type& z0, const value_type& w0, const value_type& x1, const value_type& y1, const value_type& z1, const value_type& w1, const value_type& x2, const value_type& y2, const value_type& z2, const value_type& w2)
			: m_data{ col_type(x0, y0, z0, w0), col_type(x1, y1, z1, w1), col_type(x2, y2, z2, w2) } {}

		constexpr mat(value_type&& x0, value_type&& y0, value_type&& z0, value_type&& w0, value_type&& x1, value_type&& y1, value_type&& z1, value_type&& w1, value_type&& x2, value_type&& y2, value_type&& z2, value_type&& w2)
			: m_data{ col_type(std::move(x0), std::move(y0), std::move(z0), std::move(w0)), col_type(std::move(x1), std::move(y1), std::move(z1), std::move(w1)), col_type(std::move(x2), std::move(y2), std::move(z2), std::move(w2)) } {}

		constexpr mat(const col_type& col0, const col_type& col1, const col_type& col2)
			: m_data{ col0, col1, col2 } {}

		constexpr mat(col_type&& col0, col_type&& col1, col_type&& col2)
			: m_data{ std::move(col0), std::move(col1), std::move(col2) } {}

		// Access operators

		MYL_NO_DISCARD constexpr col_type& operator[](std::size_t i) { return m_data[i]; }
		MYL_NO_DISCARD constexpr const col_type& operator[](std::size_t i) const { return m_data[i]; }

		// Comparison operators

		MYL_NO_DISCARD constexpr bool operator==(const mat& rhs) const { return m_data[0] == rhs[0] && m_data[1] == rhs[1] && m_data[2] == rhs[2]; }

		// Unary operators

		MYL_NO_DISCARD constexpr mat operator-() const { return mat{ -m_data[0], -m_data[1], -m_data[2] }; }

		// Scaler operators

		MYL_NO_DISCARD constexpr mat operator+(const value_type& rhs) const { return mat(m_data[0] + rhs, m_data[1] + rhs, m_data[2] + rhs); }
		MYL_NO_DISCARD constexpr mat operator-(const value_type& rhs) const { return mat(m_data[0] - rhs, m_data[1] - rhs, m_data[2] - rhs); }
		MYL_NO_DISCARD constexpr mat operator*(const value_type& rhs) const { return mat(m_data[0] * rhs, m_data[1] * rhs, m_data[2] * rhs); }
		MYL_NO_DISCARD constexpr mat operator/(const value_type& rhs) const { return mat(m_data[0] / rhs, m_data[1] / rhs, m_data[2] / rhs); }
		MYL_NO_DISCARD constexpr mat operator%(const value_type& rhs) const { return mat(m_data[0] % rhs, m_data[1] % rhs, m_data[2] % rhs); }

		constexpr mat& operator+=(const value_type& rhs) { m_data[0] += rhs; m_data[1] += rhs; m_data[2] += rhs; return *this; }
		constexpr mat& operator-=(const value_type& rhs) { m_data[0] -= rhs; m_data[1] -= rhs; m_data[2] -= rhs; return *this; }
		constexpr mat& operator*=(const value_type& rhs) { m_data[0] *= rhs; m_data[1] *= rhs; m_data[2] *= rhs; return *this; }
		constexpr mat& operator/=(const value_type& rhs) { m_data[0] /= rhs; m_data[1] /= rhs; m_data[2] /= rhs; return *this; }
		constexpr mat& operator%=(const value_type& rhs) { m_data[0] %= rhs; m_data[1] %= rhs; m_data[2] %= rhs; return *this; }

		// mat4x3 operators

		MYL_NO_DISCARD constexpr mat operator+(const mat& rhs) const { return mat(m_data[0] + rhs[0], m_data[1] + rhs[1], m_data[2] + rhs[2]); }
		MYL_NO_DISCARD constexpr mat operator-(const mat& rhs) const { return mat(m_data[0] - rhs[0], m_data[1] - rhs[1], m_data[2] - rhs[2]); }

		constexpr mat& operator+=(const mat& rhs) { m_data[0] += rhs[0]; m_data[1] += rhs[1]; m_data[2] += rhs[2]; return *this; }
		constexpr mat& operator-=(const mat& rhs) { m_data[0] -= rhs[0]; m_data[1] -= rhs[1]; m_data[2] -= rhs[2]; return *this; }
	};

	template<typename T> using mat4x3 = mat<4, 3, T>;

	using i8mat4x3 = mat4x3<i8>;
	using i16mat4x3 = mat4x3<i16>;
	using i32mat4x3 = mat4x3<i32>;
	using i64mat4x3 = mat4x3<i64>;

	using u8mat4x3 = mat4x3<u8>;
	using u16mat4x3 = mat4x3<u16>;
	using u32mat4x3 = mat4x3<u32>;
	using u64mat4x3 = mat4x3<u64>;

	using f32mat4x3 = mat4x3<f32>;
	using f64mat4x3 = mat4x3<f64>;
}
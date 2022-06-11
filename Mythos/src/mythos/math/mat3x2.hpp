#pragma once
#include "vec2.hpp"
#include "vec3.hpp"

namespace myl {
	template<typename T>
	struct mat<3, 2, T> {
		using value_type = T;
		using transpose_type = mat<2, 3, value_type>;
		using col_type = vec<3, value_type>;
		using row_type = vec<2, value_type>;
	private:
		col_type m_data[2];
	public:
		// Constructors

		constexpr mat() = default;

		constexpr explicit mat(const value_type& a_scalar)
			: m_data{ col_type(a_scalar), col_type(a_scalar) } {}

		constexpr mat(const value_type& x0, const value_type& y0, const value_type& z0, const value_type& x1, const value_type& y1, const value_type& z1)
			: m_data{ col_type(x0, y0, z0), col_type(x1, y1, z1) } {}

		constexpr mat(value_type&& x0, value_type&& y0, value_type&& z0, value_type&& x1, value_type&& y1, value_type&& z1)
			: m_data{ col_type(std::move(x0), std::move(y0), std::move(z0)), col_type(std::move(x1), std::move(y1), std::move(z1)) } {}

		constexpr mat(const col_type& col0, const col_type& col1)
			: m_data{ col0, col1 } {}

		constexpr mat(col_type&& col0, col_type&& col1)
			: m_data{ std::move(col0), std::move(col1) } {}

		// Access operators

		MYL_NO_DISCARD constexpr col_type& operator[](std::size_t i) { return m_data[i]; }
		MYL_NO_DISCARD constexpr const col_type& operator[](std::size_t i) const { return m_data[i]; }

		// Comparison operators

		MYL_NO_DISCARD constexpr bool operator==(const mat& rhs) const { return m_data[0] == rhs[0] && m_data[1] == rhs[1]; }

		// Unary operators

		MYL_NO_DISCARD constexpr mat operator-() const { return mat{ -m_data[0], -m_data[1] }; }

		// Scaler operators

		MYL_NO_DISCARD constexpr mat operator+(const value_type& rhs) const { return mat(m_data[0] + rhs, m_data[1] + rhs); }
		MYL_NO_DISCARD constexpr mat operator-(const value_type& rhs) const { return mat(m_data[0] - rhs, m_data[1] - rhs); }
		MYL_NO_DISCARD constexpr mat operator*(const value_type& rhs) const { return mat(m_data[0] * rhs, m_data[1] * rhs); }
		MYL_NO_DISCARD constexpr mat operator/(const value_type& rhs) const { return mat(m_data[0] / rhs, m_data[1] / rhs); }
		MYL_NO_DISCARD constexpr mat operator%(const value_type& rhs) const { return mat(m_data[0] % rhs, m_data[1] % rhs); }

		constexpr mat& operator+=(const value_type& rhs) { m_data[0] += rhs; m_data[1] += rhs; return *this; }
		constexpr mat& operator-=(const value_type& rhs) { m_data[0] -= rhs; m_data[1] -= rhs; return *this; }
		constexpr mat& operator*=(const value_type& rhs) { m_data[0] *= rhs; m_data[1] *= rhs; return *this; }
		constexpr mat& operator/=(const value_type& rhs) { m_data[0] /= rhs; m_data[1] /= rhs; return *this; }
		constexpr mat& operator%=(const value_type& rhs) { m_data[0] %= rhs; m_data[1] %= rhs; return *this; }

		// mat3x2 operators

		MYL_NO_DISCARD constexpr mat operator+(const mat& rhs) const { return mat(m_data[0] + rhs[0], m_data[1] + rhs[1]); }
		MYL_NO_DISCARD constexpr mat operator-(const mat& rhs) const { return mat(m_data[0] - rhs[0], m_data[1] - rhs[1]); }

		constexpr mat& operator+=(const mat& rhs) { m_data[0] += rhs[0]; m_data[1] += rhs[1]; return *this; }
		constexpr mat& operator-=(const mat& rhs) { m_data[0] -= rhs[0]; m_data[1] -= rhs[1]; return *this; }
	};

	template<typename T> using mat3x2 = mat<3, 2, T>;

	using i8mat3x2 = mat3x2<i8>;
	using i16mat3x2 = mat3x2<i16>;
	using i32mat3x2 = mat3x2<i32>;
	using i64mat3x2 = mat3x2<i64>;

	using u8mat3x2 = mat3x2<u8>;
	using u16mat3x2 = mat3x2<u16>;
	using u32mat3x2 = mat3x2<u32>;
	using u64mat3x2 = mat3x2<u64>;

	using f32mat3x2 = mat3x2<f32>;
	using f64mat3x2 = mat3x2<f64>;
}
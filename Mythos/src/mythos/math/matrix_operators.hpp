#pragma once
#include "matrix.hpp"

namespace myl {
	template<typename T> MYL_NO_DISCARD constexpr mat2x2<T> operator/(const mat2x2<T>& lhs, const mat2x2<T>& rhs) { return lhs * inverse(rhs); }
	template<typename T> constexpr mat2x2<T>& operator/=(const mat2x2<T>& lhs, const mat2x2<T>& rhs) { return lhs *= inverse(rhs); }

	template<typename T> MYL_NO_DISCARD constexpr mat3x3<T> operator/(const mat3x3<T>& lhs, const mat3x3<T>& rhs) { return lhs * inverse(rhs); }
	template<typename T> constexpr mat3x3<T>& operator/=(const mat3x3<T>& lhs, const mat3x3<T>& rhs) { return lhs *= inverse(rhs); }

	template<typename T> MYL_NO_DISCARD constexpr mat4x4<T> operator/(const mat4x4<T>& lhs, const mat4x4<T>& rhs) { return lhs * inverse(rhs); }
	template<typename T> constexpr mat4x4<T>& operator/=(const mat4x4<T>& lhs, const mat4x4<T>& rhs) { return lhs *= inverse(rhs); }
}
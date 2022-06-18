#pragma once
#include "geometric.hpp"

/// MYTodo: comparision operations for matrices and quats

namespace myl {
	template<typename T> MYL_NO_DISCARD constexpr bool operator<(const vec2<T>& lhs, const vec2<T>& rhs) { return dot(lhs, lhs) < dot(rhs, rhs); }
	template<typename T> MYL_NO_DISCARD constexpr bool operator>(const vec2<T>& lhs, const vec2<T>& rhs) { return dot(lhs, lhs) > dot(rhs, rhs); }
	template<typename T> MYL_NO_DISCARD constexpr bool operator<=(const vec2<T>& lhs, const vec2<T>& rhs) { return dot(lhs, lhs) <= dot(rhs, rhs); }
	template<typename T> MYL_NO_DISCARD constexpr bool operator>=(const vec2<T>& lhs, const vec2<T>& rhs) { return dot(lhs, lhs) >= dot(rhs, rhs); }

	template<typename T> MYL_NO_DISCARD constexpr bool operator<(const vec3<T>& lhs, const vec3<T>& rhs) { return dot(lhs, lhs) < dot(rhs, rhs); }
	template<typename T> MYL_NO_DISCARD constexpr bool operator>(const vec3<T>& lhs, const vec3<T>& rhs) { return dot(lhs, lhs) > dot(rhs, rhs); }
	template<typename T> MYL_NO_DISCARD constexpr bool operator<=(const vec3<T>& lhs, const vec3<T>& rhs) { return dot(lhs, lhs) <= dot(rhs, rhs); }
	template<typename T> MYL_NO_DISCARD constexpr bool operator>=(const vec3<T>& lhs, const vec3<T>& rhs) { return dot(lhs, lhs) >= dot(rhs, rhs); }

	template<typename T> MYL_NO_DISCARD constexpr bool operator<(const vec4<T>& lhs, const vec4<T>& rhs) { return dot(lhs, lhs) < dot(rhs, rhs); }
	template<typename T> MYL_NO_DISCARD constexpr bool operator>(const vec4<T>& lhs, const vec4<T>& rhs) { return dot(lhs, lhs) > dot(rhs, rhs); }
	template<typename T> MYL_NO_DISCARD constexpr bool operator<=(const vec4<T>& lhs, const vec4<T>& rhs) { return dot(lhs, lhs) <= dot(rhs, rhs); }
	template<typename T> MYL_NO_DISCARD constexpr bool operator>=(const vec4<T>& lhs, const vec4<T>& rhs) { return dot(lhs, lhs) >= dot(rhs, rhs); }
}
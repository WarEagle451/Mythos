#pragma once
#include "structure_def.hpp"

namespace myl::math {
	template<std::size_t S, typename T>
	MYL_NO_DISCARD constexpr T dot(const vec<S, T>& a, const vec<S, T>& b);
	
	template<typename T>
	MYL_NO_DISCARD constexpr T dot(const vec<2, T>& a, const vec<2, T>& b) {
		return a.x * b.x + a.y * b.y;
	}
	
	template<typename T>
	MYL_NO_DISCARD constexpr T dot(const vec<3, T>& a, const vec<3, T>& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}
	
	template<typename T>
	MYL_NO_DISCARD constexpr T dot(const vec<4, T>& a, const vec<4, T>& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	//@brief AKA magnitude
	template<std::size_t S, typename T>
	MYL_NO_DISCARD constexpr T length(const vec<S, T>& a_vec) {
		return sqrt(dot(a_vec, a_vec));
	}

	template<std::size_t S, typename T>
	MYL_NO_DISCARD constexpr T distance(const vec<S, T>& p0, const vec<S, T>& p1) {
		return length(p1 - p0);
	}

	template<std::size_t S, typename T>
	MYL_NO_DISCARD constexpr vec<S, T> normalize(const vec<S, T>& a_vec) {
		return a_vec / length(a_vec);
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec<3, T> cross(const vec<3, T>& a_vec1, const vec<3, T>& a_vec2) {
		return vec<3, T>(
			a_vec1.y * a_vec2.z - a_vec2.y * a_vec1.z,
			a_vec1.z * a_vec2.x - a_vec2.z * a_vec1.x,
			a_vec1.x * a_vec2.y - a_vec2.x * a_vec1.y);
	}
}
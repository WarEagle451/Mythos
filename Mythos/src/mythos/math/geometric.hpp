#pragma once
#include "common.hpp"
#include "structure_def.hpp"

namespace myl {
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
		return myl::sqrt(dot(a_vec, a_vec));
	}

	template<std::size_t S, typename T>
	MYL_NO_DISCARD constexpr T distance(const vec<S, T>& a, const vec<S, T>& b) {
		return length(b - a);
	}

	template<std::size_t S, typename T>
	MYL_NO_DISCARD constexpr vec<S, T> normalize(const vec<S, T>& a_vec) {
		return a_vec / length(a_vec);
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec<3, T> cross(const vec<3, T>& a, const vec<3, T>& b) {
		return vec<3, T>(
			a.y * b.z - b.y * a.z,
			a.z * b.x - b.z * a.x,
			a.x * b.y - b.x * a.y);
	}
}
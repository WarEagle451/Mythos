#pragma once
#include "mat4x4.hpp"
#include "vec3.hpp"
#include "trigonometry.hpp"
#include "geometric.hpp"

namespace myl {
	using transform = mat4x4<float>;

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> translation(const vec3<T>& position) {
		return mat4x4<T>(
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			vec4<T>(position, 1));
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> scale(const vec3<T>& scale) {
		return mat4x4<T>(
			{ scale.x, 0, 0, 0 },
			{ 0, scale.y, 0, 0 },
			{ 0, 0, scale.z, 0 },
			{ 0, 0, 0, 1 });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> euler_x(T radians) {
		T c = cos(radians);
		T s = sin(radians);

		return mat4x4<T>(
			{ 1, 0, 0, 0 },
			{ 0, c, -s, 0 },
			{ 0, s, c, 0 },
			{ 0, 0, 0, 1 });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> euler_y(T radians) {
		T c = cos(radians);
		T s = sin(radians);

		return mat4x4<T>(
			{ c, 0, s, 0 },
			{ 0, 1, 0, 0 },
			{ -s, 0, c, 0 },
			{ 0, 0, 0, 1 });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> euler_z(T radians) {
		T c = cos(radians);
		T s = sin(radians);

		return mat4x4<T>(
			{ c, -s, 0, 0 },
			{ s, c, 0, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 1 });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> euler_xyz(const vec3<T>& radians) {
		return euler_x(radians.x) * euler_y(radians.y) * euler_z(radians.z);
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> forward_lh(const mat4x4<T>& mat) {
		return normalize(vec3<T>{ mat[0][2], mat[1][2], mat[2][2] });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> backward_lh(const mat4x4<T>& mat) {
		return -forward_lh(mat);
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> up_lh(const mat4x4<T>& mat) {
		return normalize(vec3<T>{ mat[0][1], mat[1][1], mat[2][1] });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> down_lh(const mat4x4<T>& mat) {
		return -up_lh(mat);
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> right_lh(const mat4x4<T>& mat) {
		return normalize(vec3<T>{ mat[0][0], mat[1][0], mat[2][0] });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> left_lh(const mat4x4<T>& mat) {
		return -right(mat);
	}

	/// MYTodo: Need to imple right handed versions of functions
#ifdef MYL_FORCE_RIGHT_HANDED
#	error "Right handed functions are not implemented"

	template<typename T> MYL_NO_DISCARD constexpr vec3<T> forward(const mat4x4<T>& mat)		{ return forward_rh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> backward(const mat4x4<T>& mat)	{ return backward_rh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> up(const mat4x4<T>& mat)			{ return up_rh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> down(const mat4x4<T>& mat)		{ return down_rh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> right(const mat4x4<T>& mat)		{ return right_rh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> left(const mat4x4<T>& mat)		{ return left_rh(mat); }
#else
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> forward(const mat4x4<T>& mat)		{ return forward_lh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> backward(const mat4x4<T>& mat)	{ return backward_lh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> up(const mat4x4<T>& mat)			{ return up_lh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> down(const mat4x4<T>& mat)		{ return down_lh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> right(const mat4x4<T>& mat)		{ return right_lh(mat); }
	template<typename T> MYL_NO_DISCARD constexpr vec3<T> left(const mat4x4<T>& mat)		{ return left_lh(mat); }
#endif
}
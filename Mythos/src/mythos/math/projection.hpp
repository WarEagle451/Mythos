#pragma once
#include "mat4x4.hpp"
#include "trigonometry.hpp"
#include "vec3.hpp"
#include "geometric.hpp"

namespace myl {
	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> orthographic_lh(T left, T right, T bottom, T top, T a_near, T a_far) {
		const T rl = static_cast<T>(right - left);
		const T tb = static_cast<T>(top - bottom);
		const T fn = static_cast<T>(a_far - a_near);

		return mat4x4<T>(
			{ static_cast<T>(2) / rl, 0, 0, 0 },
			{ 0, static_cast<T>(2) / tb, 0, 0 },
			{ 0, 0, static_cast<T>(2) / fn, 0 },
			{ -(right + left) / rl, -(top + bottom) / tb, -(a_far + a_near) / fn, 1 });
	}
	
	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> perspective_lh(T fov_radians, T aspect, T a_near, T a_far) {
		const T half_tan_fov = tan(fov_radians * static_cast<T>(.5f));
	
		return mat4x4<T>(
			{ static_cast<T>(1) / (aspect * half_tan_fov), 0, 0, 0 },
			{ 0, static_cast<T>(1) / half_tan_fov, 0, 0 },
			{ 0, 0, (a_far + a_near) / (a_far - a_near), static_cast<T>(1) },
			{ 0, 0, -(static_cast<T>(2) * a_far * a_near) / (a_far - a_near), 1 });
	}
	
	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> look_at_lh(const vec3<T>& target, const vec3<T>& eye, const vec3<T>& up) {
		vec3<T> z_axis(normalize(target - eye));
		vec3<T> x_axis(normalize(cross(up, z_axis)));
		vec3<T> y_axis(cross(z_axis, x_axis));

		return mat4x4<T>(
			{ x_axis.x, y_axis.x, z_axis.x, 0 },
			{ x_axis.y, y_axis.y, z_axis.y, 0 },
			{ x_axis.z, y_axis.z, z_axis.z, 0 },
			{ -dot(x_axis, eye), -dot(y_axis, eye), -dot(z_axis, eye), 1 });
	}

	/// MYTodo: Need to imple right handed versions of functions
#ifdef MYL_FORCE_RIGHT_HANDED
#	error "Right handed functions are not implemented"

	template<typename T> MYL_NO_DISCARD constexpr mat4x4<T> orthographic(T left, T right, T bottom, T top, T a_near, T a_far) { return orthographic_rh(left, right, bottom, top, a_near, a_far); }
	template<typename T> MYL_NO_DISCARD constexpr mat4x4<T> perspective(T fov_radians, T aspect, T a_near, T a_far) { return perspective_rh(fov_radians, aspect, a_near, a_far); }
	template<typename T> MYL_NO_DISCARD constexpr mat4x4<T> look_at(const vec3<T>& target, const vec3<T>& eye, const vec3<T>& up) { return look_at_rh(target, eye, up); }
#else
	template<typename T> MYL_NO_DISCARD constexpr mat4x4<T> orthographic(T left, T right, T bottom, T top, T a_near, T a_far)		{ return orthographic_lh(left, right, bottom, top, a_near, a_far); }
	template<typename T> MYL_NO_DISCARD constexpr mat4x4<T> perspective(T fov_radians, T aspect, T a_near, T a_far)					{ return perspective_lh(fov_radians, aspect, a_near, a_far); }
	template<typename T> MYL_NO_DISCARD constexpr mat4x4<T> look_at(const vec3<T>& target, const vec3<T>& eye, const vec3<T>& up)	{ return look_at_lh(target, eye, up); }
#endif
}
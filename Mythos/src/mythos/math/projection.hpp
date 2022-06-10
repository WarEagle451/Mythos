#pragma once
#include "mat4x4.hpp"
#include "trigonometry.hpp"
#include "vec3.hpp"
#include "geometric.hpp"

namespace myl {
	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> orthographic(T left, T right, T bottom, T top, T a_near, T a_far) {
		T lr = static_cast<T>(1) / (left - right);
		T bt = static_cast<T>(1) / (bottom - top);
		T nf = static_cast<T>(1) / (a_near - a_far);

		return mat4x4<T>(
			{ -static_cast<T>(2) * lr, 0, 0, 0 },
			{ 0, -static_cast<T>(2) * bt, 0, 0 },
			{ 0, 0, static_cast<T>(2) * lr, 0 },
			{ (left + right) * lr, (bottom + top) * bt, (a_near + a_far) * nf, 1 });
	}
	
	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> perspective(T fov_radians, T aspect_ratio, T a_near, T a_far) {
		T half_tan_fov = tan(fov_radians * static_cast<T>(.5f));
	
		return mat4x4<T>(
			{ static_cast<T>(1) / (aspect_ratio * half_tan_fov), 0, 0, 0 },
			{ 0, static_cast<T>(1) / half_tan_fov, 0, 0 },
			{ 0, 0, -((a_far + a_near) / (a_far - a_near)), -static_cast<T>(1) },
			{ 0, 0, -((static_cast<T>(2) * a_far * a_near) / (a_far - a_near)), 1 });
	}
	
	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> look_at(const vec3<T>& position, const vec3<T> target, const vec3<T>& up) { 
		vec3<T> z_axis{
			.x = target.x - position.x,
			.y = target.y - position.y,
			.z = target.z - position.z
		};
	
		z_axis = normalize(z_axis);
		vec3<T> x_axis = normalize(cross(z_axis, up));
		vec3<T> y_axis = cross(x_axis, z_axis);
	
		return mat4x4<T>(
			{ x_axis.x, y_axis.x, -z_axis.x, 0 },
			{ x_axis.y, y_axis.y, -z_axis.y, 0 },
			{ x_axis.z, y_axis.z, -z_axis.z, 0 },
			{ -dot(x_axis, position), -dot(y_axis, position), dot(z_axis, position), 1});
	}
}
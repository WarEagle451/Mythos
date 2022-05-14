#pragma once
#include "mat4x4.hpp"
#include "vec3.hpp"
#include "trigonometry.hpp"
#include "geometric.hpp"

/// MYBug: This will only be correct if kohi has the matrices as 0-3 = column 1, etc, if 0-3 = row 1 might have issues

namespace myl {
	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> matrix_translation(const vec3<T>& position) {
		return mat4x4<T>( /// MYTodo: Make sure this is good for a left handed cord system
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			vec4<T>(position, 1));
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> matrix_scale(const vec3<T>& scale) {
		return mat4x4<T>( /// MYTodo: Make sure this is good for a left handed cord system
			{ scale.x, 0, 0, 0 },
			{ 0, scale.y, 0, 0 },
			{ 0, 0, scale.z, 0 },
			{ 0, 0, 0, 1 });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> matrix_euler_x(T radians) {
		T c = cos(radians);
		T s = sin(radians);

		return mat4x4<T>( /// MYTodo: Make sure this is good for a left handed cord system
			{ 1, 0, 0, 0 },
			{ 0, c, s, 0 },
			{ 0, -s, c, 0 },
			{ 0, 0, 0, 1 });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> matrix_euler_y(T radians) {
		T c = cos(radians);
		T s = sin(radians);

		return mat4x4<T>( /// MYTodo: Make sure this is good for a left handed cord system
			{ c, 0, -s, 0 },
			{ 0, 1, 0, 0 },
			{ s, 0, c, 0 },
			{ 0, 0, 0, 1 });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> matrix_euler_z(T radians) {
		T c = cos(radians);
		T s = sin(radians);

		return mat4x4<T>( /// MYTodo: Make sure this is good for a left handed cord system
			{ c, s, 0, 0 },
			{ -s, c, 0, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 1 });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr mat4x4<T> matrix_euler_rotation(const vec3<T>& radians) { /// MYTodo: Make sure this is good for a left handed cord system
		return matrix_euler_x(radians.x) * matrix_euler_y(radians.y) * matrix_euler_z(radians.z);
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> forward(const mat4x4<T>& mat) { /// MYTodo: Make sure this is good for a left handed cord system
		return normalize(-vec3<T>{ mat[0][2], mat[1][2], mat[2][2] });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> backward(const mat4x4<T>& mat) { /// MYTodo: Make sure this is good for a left handed cord system
		return normalize({ mat[0][2], mat[1][2], mat[2][2] });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> up(const mat4x4<T>& mat) { /// MYTodo: Make sure this is good for a left handed cord system
		return normalize({ mat[0][1], mat[1][1], mat[2][1] });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> down(const mat4x4<T>& mat) { /// MYTodo: Make sure this is good for a left handed cord system
		return normalize(-vec3<T>{ mat[0][1], mat[1][1], mat[2][1] });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> left(const mat4x4<T>& mat) { /// MYTodo: Make sure this is good for a left handed cord system
		return normalize(-vec3<T>{ mat[0][0], mat[1][0], mat[2][0] });
	}

	template<typename T>
	MYL_NO_DISCARD constexpr vec3<T> right(const mat4x4<T>& mat) { /// MYTodo: Make sure this is good for a left handed cord system
		return normalize({ mat[0][0], mat[1][0], mat[2][0] });
	}
}
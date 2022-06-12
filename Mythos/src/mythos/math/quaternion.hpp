#pragma once
#include "common.hpp"
#include "structure_def.hpp" // mat
#include "trigonometry.hpp"

#include <utility> // std::move

namespace myl {
	template<typename T> struct quat; // fwd declaration because of funcs below and normalize has to be called in a quat constructor

	template<typename T>
	MYL_NO_DISCARD constexpr T dot(const quat<T>& a, const quat<T>& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	template<typename T>
	MYL_NO_DISCARD constexpr T normal(const quat<T>& q) {
		return myl::sqrt(dot(q, q));
	}

	template<typename T>
	MYL_NO_DISCARD constexpr quat<T> normalize(const quat<T>& q) {
		return q / normal(q);
	}

	template<typename T>
	MYL_NO_DISCARD constexpr quat<T> conjugate(const quat<T>& q) {
		return quat<T>{ -q.x, -q.y, -q.z, q.w };
	}

	template<typename T>
	MYL_NO_DISCARD constexpr quat<T> inverse(const quat<T>& q) {
		return normalize(conjugate(q));
	}

	template<typename T> /// MYTodo: quats do not have handedness, but matrices do, this might not work with left handed matrices
	MYL_NO_DISCARD constexpr mat4x4<T> quat_to_matrix(const quat<T>& q) { /// MYTodo: Make this a mat4x4 constructor
		// https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix

		quat n = normalize(q);
		constexpr T _1 = static_cast<T>(1);
		constexpr T _2 = static_cast<T>(2);

		return mat4x4<T>(
			{ _1 - _2 * n.y * n.y - _2 * n.z * n.z, _2 * n.x * n.y - _2 * n.z * n.w, _2 * n.x * n.z + _2 * n.y * n.w, 0 },
			{ _2 * n.x * n.y + _2 * n.z * n.w, _1 - _2 * n.x * n.x - _2 * n.z * n.z, _2 * n.y * n.z - _2 * n.x * n.w, 0 },
			{ _2 * n.x * n.z - _2 * n.y * n.w, _2 * n.y * n.z + _2 * n.x * n.w, _1 - _2 * n.x * n.x - _2 * n.y * n.y, 0 },
			{ 0, 0, 0, 1 });
	}

	template<typename T> /// MYTodo: quats do not have handedness, but matrices do, this might not work with left handed matrices
	MYL_NO_DISCARD constexpr mat4x4<T> quat_to_rotation_matrix(const quat<T>& q, const vec3<T>& center) { /// MYTodo: Make this a mat4x4 constructor
		constexpr T _2 = static_cast<T>(2);
		mat4x4<T> out{};

		/*00*/ out[0][0] = (q.x * q.x) - (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
		/*01*/ out[0][1] = _2 * ((q.x * q.y) + (q.z * q.w));
		/*02*/ out[0][2] = _2 * ((q.x * q.z) - (q.y * q.w));
		/*03*/ out[0][3] = center.x - center.x * out[0][0] - center.y * out[0][1] - center.z * out[0][2];

		/*04*/ out[1][0] = _2 * ((q.x * q.y) - (q.z * q.w));
		/*05*/ out[1][1] = -(q.x * q.x) + (q.y * q.y) - (q.z * q.z) + (q.w * q.w);
		/*06*/ out[1][2] = _2 * ((q.y * q.z) + (q.x * q.w));
		/*07*/ out[1][3] = center.y - center.x * out[1][0] - center.y * out[1][1] - center.z * out[1][2];

		/*08*/ out[2][0] = _2 * ((q.x * q.z) + (q.y * q.w));
		/*09*/ out[2][1] = _2 * ((q.y * q.z) - (q.x * q.w));
		/*10*/ out[2][2] = -(q.x * q.x) - (q.y * q.y) + (q.z * q.z) + (q.w * q.w);
		/*11*/ out[2][3] = center.z - center.x * out[2][0] - center.y * out[2][1] - center.z * out[2][2];

		out[3] = { 0, 0, 0, 1 };

		return out;
	}

	//@brief Spherical linear interpolation
	template<typename T>
	MYL_NO_DISCARD constexpr quat<T> slerp(const quat<T>& a, const quat<T>& b, f32 percentage = .5f) {
		// Source: https://en.wikipedia.org/wiki/Slerp
		// Only unit quaternions are valid rotations
		// Normalize to avoid undefined behavior
		quat<T> v0 = normalize(a);
		quat<T> v1 = normalize(b);

		// Compute the cosine of the angle between the two vectors
		f32 dot = dot(v0, v1);

		// If the dot product is negative, slerp won't take the shorter path
		// Note that v1 and -v1 are equivalent when the negation is applied to all four components
		// Fix by reversing one quaternion
		if (dot < 0) {
			v1.x = -v1.x;
			v1.y = -v1.y;
			v1.z = -v1.z;
			v1.w = -v1.w;
			dot = -dot;
		}

		constexpr f32 DOT_THRESHOLD = 0.9995f;
		if (dot > DOT_THRESHOLD) { // If the inputs are too close for comfort, linearly interpolate and normalize the result
			quat<T> out{
				.x = v0.x + ((v1.x - v0.x) * percentage),
				.y = v0.y + ((v1.y - v0.y) * percentage),
				.z = v0.z + ((v1.z - v0.z) * percentage),
				.w = v0.w + ((v1.w - v0.w) * percentage)
			};

			return normalize(out);
		}

		// Since dot is in range [0, DOT_THRESHOLD], acos is safe
		f32 theta_0 = myl::acos(dot);			// theta_0 = angle between input vectors
		f32 theta = theta_0 * percentage;		// theta = angle between v0 and result
		f32 sin_theta = myl::sin(theta);		// compute this value only once
		f32 sin_theta_0 = myl::sin(theta_0);	// compute this value only once

		f32 s0 = myl::cos(theta) - dot * sin_theta / sin_theta_0; // == sin(theta_0 - theta) / sin(theta_0)
		f32 s1 = sin_theta / sin_theta_0;

		return quat<T>{
			.x = (v0.x * s0) + (v1.x * s1),
			.y = (v0.y * s0) + (v1.y * s1),
			.z = (v0.z * s0) + (v1.z * s1),
			.w = (v0.w * s0) + (v1.w * s1)
		};
	}

	template<typename T>
	struct quat {
		using value_type = T;

		union {
			value_type data[4];
			struct { value_type x, y, z, w; };
		};

		// Constructors

		constexpr quat() = default;

		constexpr explicit quat(const value_type& a_scalar)
			: data{ a_scalar, a_scalar, a_scalar, a_scalar } {}

		constexpr quat(const value_type& a_x, const value_type& a_y, const value_type& a_z, const value_type& a_w)
			: data{ a_x, a_y, a_z, a_w } {}

		constexpr quat(value_type&& a_x, value_type&& a_y, value_type&& a_z, value_type&& a_w)
			: data{ std::move(a_x), std::move(a_y), std::move(a_z), std::move(a_w) } {}

		constexpr explicit quat(const vec3<value_type>& a_angles) {
			const value_type half = static_cast<value_type>(.5f);
			const vec3<value_type> c{ cos(a_angles.x) * half, cos(a_angles.y) * half, cos(a_angles.z) * half };
			const vec3<value_type> s{ sin(a_angles.x) * half, sin(a_angles.y) * half, sin(a_angles.z) * half };

			x = s.x * c.y * c.z - c.x * s.y * s.z;
			y = c.x * s.y * c.z + s.x * c.y * s.z;
			z = c.x * c.y * s.z - s.x * s.y * c.z;
			w = c.x * c.y * c.z + s.x * s.y * s.z;
		}

		//@brief Constructs a quat from an axis angle
		constexpr explicit quat(const vec3<value_type>& a_axis, value_type a_angle, bool a_normalize) {
			const value_type half_angle = a_angle * static_cast<value_type>(.5f);
			const value_type s = myl::sin(half_angle);
			const value_type c = myl::cos(half_angle);

			x = s * a_axis.x;
			y = s * a_axis.y;
			z = s * a_axis.z;
			w = c;

			if (a_normalize)
				*this = normalize(*this);
		}

		// Utilities
		
		MYL_NO_DISCARD static constexpr quat identity() { return quat{ 0, 0, 0, 1 }; }
		
		// Access operators
		
		MYL_NO_DISCARD constexpr value_type& operator[](std::size_t i) { return data[i]; }
		MYL_NO_DISCARD constexpr const value_type& operator[](std::size_t i) const { return data[i]; }

		// Comparison operators
		
		MYL_NO_DISCARD constexpr bool operator==(const quat& rhs) const { return x == rhs.x && y = rhs.y && z == rhs.z && w == rhs.w; }

		// Unary operators
		
		MYL_NO_DISCARD constexpr quat operator-() const { return quat{ -x, -y, -z, -w }; }

		// Scaler operators

		MYL_NO_DISCARD constexpr quat operator+(const value_type& rhs) const { return quat{ x + rhs, y + rhs, z + rhs, w + rhs }; }
		MYL_NO_DISCARD constexpr quat operator-(const value_type& rhs) const { return quat{ x - rhs, y - rhs, z - rhs, w - rhs }; }
		MYL_NO_DISCARD constexpr quat operator*(const value_type& rhs) const { return quat{ x * rhs, y * rhs, z * rhs, w * rhs }; }
		MYL_NO_DISCARD constexpr quat operator/(const value_type& rhs) const { return quat{ x / rhs, y / rhs, z / rhs, w / rhs }; }
		MYL_NO_DISCARD constexpr quat operator%(const value_type& rhs) const { return quat{ x % rhs, y % rhs, z % rhs, w % rhs }; }

		constexpr quat& operator+=(const value_type& rhs) { x += rhs; y += rhs; z += rhs; w += rhs; return *this; }
		constexpr quat& operator-=(const value_type& rhs) { x -= rhs; y -= rhs; z -= rhs; w -= rhs; return *this; }
		constexpr quat& operator*=(const value_type& rhs) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return *this; }
		constexpr quat& operator/=(const value_type& rhs) { x /= rhs; y /= rhs; z /= rhs; w /= rhs; return *this; }
		constexpr quat& operator%=(const value_type& rhs) { x %= rhs; y %= rhs; z %= rhs; w %= rhs; return *this; }

		// quat operators

		/// MYTodo: quat +, -, /, %

		MYL_NO_DISCARD constexpr quat operator*(const quat& rhs) const {
			return quat{
				.x = x * rhs.w + y * rhs.z - z * rhs.y + w * rhs.x,
				.y = -x * rhs.y + y * rhs.w + z * rhs.x + w * rhs.y,
				.z = x * rhs.y - y * rhs.x + z * rhs.w + w * rhs.z,
				.w = -x * rhs.x - y * rhs.y - z * rhs.z + w * rhs.w
			};
		}

		constexpr quat& operator*=(const quat& rhs) { return *this = (*this * rhs); }
	};

	using i8quat = quat<i8>;
	using i16quat = quat<i16>;
	using i32quat = quat<i32>;
	using i64quat = quat<i64>;

	using u8quat = quat<u8>;
	using u16quat = quat<u16>;
	using u32quat = quat<u32>;
	using u64quat = quat<u64>;

	using f32quat = quat<f32>;
	using f64quat = quat<f64>;
}
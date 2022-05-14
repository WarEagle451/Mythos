#pragma once
#include <testbed/test_manager.hpp>

#include <mythos/math.hpp>

namespace tb {
	static constexpr bool square_root() {
		constexpr f64 answer = 3.3;
		return static_cast<f32>(answer) == static_cast<f32>(myl::sqrt(10.89)); // Casting to remove f64 errors
	}

	static constexpr bool degrees() {
		constexpr f64 answer = 57.2958;
		return static_cast<f32>(answer) == static_cast<f32>(myl::degrees(1.0));  // Casting to remove f64 errors
	}

	static constexpr bool radians() {
		constexpr f64 answer = 1;
		return static_cast<f32>(answer) == static_cast<f32>(myl::radians(57.2958));  // Casting to remove f64 errors
	}

	static constexpr bool sin() { constexpr f64 answer = 1; return answer == myl::sin(90.f); }
	static constexpr bool cos() { constexpr f64 answer = 1; return answer == myl::cos(0.f); }
	static constexpr bool tan() { constexpr f64 answer = 1; return answer == myl::tan(45.f); }

	static constexpr bool asin() { constexpr f64 answer = 90; return answer == myl::asin(1.f); }
	static constexpr bool acos() { constexpr f64 answer = 0; return answer == myl::acos(1.f); }
	static constexpr bool atan() { constexpr f64 answer = 45; return answer == myl::atan(1.f); }

	static constexpr bool dot_vec2() {
		constexpr f64 answer = 22;
		return answer == myl::dot(myl::f64vec2{ 4, 5 }, myl::f64vec2{ 8, -2 });
	}

	static constexpr bool dot_vec3() {
		constexpr f64 answer = -546;
		return answer == myl::dot(myl::f64vec3{ 4, 6, -9 }, myl::f64vec3{ -3, 4, 62 });
	}

	static constexpr bool dot_vec4() {
		constexpr f64 answer = 17;
		return answer == myl::dot(myl::f64vec4{ 8, -9, 0, 11 }, myl::f64vec4{ -3, 4, 31, 7 });
	}

	static constexpr bool length() {
		constexpr f64 answer = 13;
		return answer == myl::length(myl::f64vec3{ 4, 3, 12});
	}

	static constexpr bool normalize() {
		constexpr myl::f64vec2 answer{ 0.8, 0.6 };
		return answer == myl::normalize(myl::f64vec2{ 4, 3 });
	}

	static constexpr bool cross() {
		constexpr myl::f64vec3 answer{ 18, -46, -3 };
		return answer == myl::cross(myl::f64vec3{ 7, 3, -4 }, myl::f64vec3{ 1, 0, 6 });
	}

	/// matrix, transform, quaternion, projection

	void add_math_tests(test_manager& manager) {
		manager.add("Square Root", square_root);
		manager.add("Degrees", degrees);
		manager.add("Radians", radians);
		manager.add("Sin", sin);
		manager.add("Cos", cos);
		manager.add("Tan", tan);
		manager.add("ASin", asin);
		manager.add("ACos", acos);
		manager.add("ATan", atan);
		manager.add("Dot Vec2", dot_vec2);
		manager.add("Dot Vec3", dot_vec3);
		manager.add("Dot Vec4", dot_vec4);
		manager.add("Length", length);
		manager.add("Normalize", normalize);
		manager.add("Cross", cross);
	}
}
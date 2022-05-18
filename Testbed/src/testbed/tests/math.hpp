#pragma once
#include <testbed/test_manager.hpp>

#include <mythos/math.hpp>

#include <mythos/core/log.hpp>

// Volatile makes the constexpr functions run at runtime instead of compile time

namespace tb {
	static bool approx() {
		constexpr volatile f64 param = 3.3000000000000003;
		return myl::approx(3.3, param);
	}

	static bool square_root() {
		constexpr f64 answer = 3.3;
		constexpr volatile f64 param = 10.89;
		return myl::approx(answer, myl::sqrt(param));
	}

	static bool degrees() {
		constexpr f64 answer = 180.0 / myl::constants::pi<f64>;
		constexpr volatile f64 param = 1.0;
		return myl::approx(answer, myl::degrees(param));
	}

	static bool radians() {
		constexpr f64 answer = 1;
		constexpr volatile f64 param = myl::radians(180.0 / myl::constants::pi<f64>);
		return myl::approx(answer, param);
	}

	static bool sin() { constexpr volatile f64 param = 90.0; return 1 == myl::sin(param); }
	static bool cos() { constexpr volatile f64 param = 0.0; return 1 == myl::cos(param); }
	static bool tan() { constexpr volatile f64 param = 45.0; return 1 == myl::tan(param); }

	static bool asin() { constexpr volatile f64 param = 1.0; return 90 == myl::asin(param); }
	static bool acos() { constexpr volatile f64 param = 1.0; return 0 == myl::acos(param); }
	static bool atan() { constexpr volatile f64 param = 1.0; return 45 == myl::atan(param); }

	static bool dot_vec2() {
		constexpr f64 answer = 22;
		constexpr volatile myl::f64vec2 param1{ 4, 5 };
		constexpr volatile myl::f64vec2 param2{ 8, -2 };
		return answer == myl::dot(const_cast<myl::f64vec2&>(param1), const_cast<myl::f64vec2&>(param2));
	}

	static bool dot_vec3() {
		constexpr f64 answer = -546;
		constexpr volatile myl::f64vec3 param1{ 4, 6, -9 };
		constexpr volatile myl::f64vec3 param2{ -3, 4, 62 };
		return answer == myl::dot(const_cast<myl::f64vec3&>(param1), const_cast<myl::f64vec3&>(param2));
	}

	static bool dot_vec4() {
		constexpr f64 answer = 17;
		constexpr volatile myl::f64vec4 param1{ 8, -9, 0, 11 };
		constexpr volatile myl::f64vec4 param2{ -3, 4, 31, 7 };
		return answer == myl::dot(const_cast<myl::f64vec4&>(param1), const_cast<myl::f64vec4&>(param2));
	}

	static bool length() {
		constexpr f64 answer = 13;
		constexpr volatile myl::f64vec3 param{ 4, 3, 12 };
		return answer == myl::length(const_cast<myl::f64vec3&>(param));
	}

	static bool normalize() {
		constexpr myl::f64vec2 answer{ 0.8, 0.6 };
		constexpr volatile myl::f64vec2 param{ 4, 3 };
		return answer == myl::normalize(const_cast<myl::f64vec2&>(param));
	}

	static bool cross() {
		constexpr myl::f64vec3 answer{ 18, -46, -3 };
		constexpr volatile myl::f64vec3 param1{ 7, 3, -4 };
		constexpr volatile myl::f64vec3 param2{ 1, 0, 6 };
		return answer == myl::cross(const_cast<myl::f64vec3&>(param1), const_cast<myl::f64vec3&>(param2));
	}

	static bool lerp() { return 6.0 == myl::lerp(0.0, 10.0, 0.6); }
	static bool inverse_lerp() { return 0.6 == myl::inverse_lerp<f64>(0.0, 10.0, 6.0); }

	/// matrix, transform, quaternion, projection

	void add_math_tests(test_manager& manager) {
		manager.add("Approx", approx);
		manager.add("Lerp", lerp);
		manager.add("Inverse Lerp", inverse_lerp);
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
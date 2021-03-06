#pragma once
#include <mythos/defines.hpp>

namespace myl {
	struct timestep {
	private:
		f64 time;
	public:
		MYL_API constexpr timestep(f64 a_time = 0.0)
			: time(a_time) {}

		MYL_API constexpr timestep& operator+=(const timestep& rhs) { time += rhs; return *this; }

		MYL_API constexpr operator f64() const { return time; }
	};
}
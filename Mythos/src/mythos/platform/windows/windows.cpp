#include <mythos/platform/detection.hpp>
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/timestep.hpp>

#	include <windows.h>

namespace myl {
	static f64 s_clock_frequency;
	static LARGE_INTEGER s_start_time;

	f64 get_time() {
		LARGE_INTEGER now;
		QueryPerformanceCounter(&now);
		return static_cast<f64>(now.QuadPart - s_start_time.QuadPart) * s_clock_frequency;
	}

	void setup_clock() {
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		s_clock_frequency = 1.0 / static_cast<f64>(frequency.QuadPart);
		QueryPerformanceCounter(&s_start_time);
	}
}
#endif
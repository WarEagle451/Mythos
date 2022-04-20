#include "window.hpp"

#include <mythos/platform/detection.hpp>
#include <mythos/platform/windows/win_window.hpp>

namespace myl {
	std::unique_ptr<window> window::create(const config& a_config) {
#ifdef MYL_PLATFORM_WINDOWS
		return std::make_unique<windows::window>(a_config);
#endif
	}
}
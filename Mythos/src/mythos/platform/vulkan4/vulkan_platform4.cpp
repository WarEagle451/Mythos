#include "vulkan_platform.hpp"
#include "vulkan_utils.hpp"

#include <mythos/platform/detection.hpp>

#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/app.hpp>
#	include <mythos/platform/windows/win_window.hpp>

#	include <vulkan/vulkan_win32.h>

namespace myl::vulkan4 {
	void required_platform_extensions(std::vector<const char*>* extensions) {
		extensions->push_back("VK_KHR_win32_surface");
	}

	VkSurfaceKHR platform_create_surface(VkInstance instance) {
		auto window = static_cast<windows::window*>(myl::app::get().window()); // Window is set before renderer is initailized

		VkWin32SurfaceCreateInfoKHR create_info{
			.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
			.hinstance = window->module(),
			.hwnd = static_cast<HWND>(window->native())
		};

		VkSurfaceKHR surface{};
		MYL_VK_ASSERT(vkCreateWin32SurfaceKHR, instance, &create_info, VK_NULL_HANDLE, &surface);
		return surface;
	}
}
#endif
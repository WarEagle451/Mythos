#include <mythos/platform/detection.hpp>
#include <mythos/platform/vulkan/vulkan_platform.hpp>
#include <mythos/platform/vulkan/vulkan_context.hpp>

#include <mythos/core/except.hpp>
#ifdef MYL_PLATFORM_WINDOWS
#	include <mythos/core/app.hpp>
#	include <mythos/platform/windows/win_window.hpp>

#	include <vulkan/vulkan_win32.h>

namespace myl::vulkane {
	void platform_required_extensions(std::vector<const char*>* a_extensions) {
		a_extensions->push_back("VK_KHR_win32_surface");
	}

	VkSurfaceKHR platform_create_surface(VkInstance& a_instance) {
		windows::window* window = static_cast<windows::window*>(app::get().window()); // Window and app are set before the renderer is initialized

		VkWin32SurfaceCreateInfoKHR create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		create_info.hinstance = window->module();
		create_info.hwnd = static_cast<HWND>(window->native());

		VkSurfaceKHR surface{};
		if (vkCreateWin32SurfaceKHR(a_instance, &create_info, nullptr, &surface) != VK_SUCCESS)
			throw core_runtime_error("Vulkan surface creation failed");
		return surface;
	}
}
#endif
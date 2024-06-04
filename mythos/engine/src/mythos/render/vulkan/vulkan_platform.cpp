#include <mythos/render/vulkan/vulkan_platform.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

#ifdef MYL_OS_WINDOWS
#   include <mythos/platform/windows/win_window.hpp>

#   include <vulkan/vulkan_win32.h>
#endif

namespace myth::vulkan {
    auto get_platform_required_extensions(std::vector<const char*>* extensions) -> void {
#ifdef MYL_OS_WINDOWS
        extensions->emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
    }

    auto create_surface(VkSurfaceKHR* surface, VkInstance instance, myth::window* target, VkAllocationCallbacks* allocator) -> void {
#ifdef MYL_OS_WINDOWS
        win::window* w = static_cast<win::window*>(target);
        VkWin32SurfaceCreateInfoKHR create_info{
            .sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            //.pNext = ,
            //.flags = ,
            .hinstance = w->instance(),
            .hwnd      = w->handle()
        };

        MYTHOS_VULKAN_VERIFY(vkCreateWin32SurfaceKHR, instance, &create_info, allocator, surface);
#endif
    }

    MYL_NO_DISCARD auto get_physical_device_presentation_support_khr(VkPhysicalDevice physical_device, uint32_t queue_family_index) -> VkBool32 {
#ifdef MYL_OS_WINDOWS
        return vkGetPhysicalDeviceWin32PresentationSupportKHR(physical_device, queue_family_index);
#else
        return VK_FALSE;
#endif
    }
}

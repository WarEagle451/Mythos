#include <mythos/render/vulkan2/vulkan_swapchain.hpp>

namespace myth::vulkan2 {
    auto swapchain::query_support(swapchain_support_details* details, VkPhysicalDevice physical_device, VkSurfaceKHR surface) -> void {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details->capabilities);

        uint32_t format_count{};
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, VK_NULL_HANDLE);
        details->formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details->formats.data());

        uint32_t present_mode_count{};
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, VK_NULL_HANDLE);
        details->present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details->present_modes.data());
    }
}

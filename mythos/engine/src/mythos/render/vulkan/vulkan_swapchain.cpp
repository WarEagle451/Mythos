#include <mythos/render/vulkan/vulkan_swapchain.hpp>

#include <myl/algorithm.hpp>

#include <limits>

namespace myth::vulkan {
    MYL_NO_DISCARD constexpr auto choose_surface_format(const std::vector<VkSurfaceFormatKHR>& available_formats) -> VkSurfaceFormatKHR {
        if (available_formats.empty())
            return VkSurfaceFormatKHR{
                .format     = VK_FORMAT_MAX_ENUM,
                .colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR
            };

        for (const auto& format : available_formats)
            if (format.format == VK_FORMAT_B8G8R8A8_SRGB && // Preferred format
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return format;
        return available_formats[0]; /// MYTODO: if the above fails ranking how good formats are to choose would be a good idea
    }

    MYL_NO_DISCARD constexpr auto choose_present_mode(const std::vector<VkPresentModeKHR>& available_modes) -> VkPresentModeKHR {
        for (const auto& mode : available_modes)
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    MYL_NO_DISCARD constexpr auto choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, window& window) -> VkExtent2D {
        if (capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max()) {
            auto fb_size = window.framebuffer_size();
            return VkExtent2D{
                .width  = myl::clamp(static_cast<uint32_t>(fb_size.w), capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
                .height = myl::clamp(static_cast<uint32_t>(fb_size.h), capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
            };
        }
        else
            return capabilities.currentExtent;
    }

    auto swapchain::query_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, swapchain_support_details* details) -> void {
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details->capabilities);

        uint32_t format_count = 0;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, VK_NULL_HANDLE);
        details->formats.resize(format_count);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details->formats.data());

        uint32_t present_mode_count = 0;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, VK_NULL_HANDLE);
        details->present_modes.resize(present_mode_count);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details->present_modes.data());
    }

    MYL_NO_DISCARD swapchain::swapchain(context& context, window& window)
        : m_context{ context } {
        swapchain_support_details ssd{};
        query_support(m_context.physical_device(), m_context.surface(), &ssd);

        VkSurfaceFormatKHR surface_format = choose_surface_format(ssd.formats);
        VkPresentModeKHR present_mode = choose_present_mode(ssd.present_modes);
        VkExtent2D extent = choose_swap_extent(ssd.capabilities, window);
    }

    swapchain::~swapchain() {

    }
}

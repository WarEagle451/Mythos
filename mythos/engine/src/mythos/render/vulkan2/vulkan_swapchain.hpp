#pragma once
#include <vulkan/vulkan.h>

#include <vector>

namespace myth::vulkan2 {
    struct swapchain_support_details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    class swapchain {
    public:
        ///struct create_info {
        ///    device& device;
        ///    const VkExtent2D& extent;
        ///};
        ///
        ///static auto create(swapchain* handle, const create_info& create_info) -> void;
        ///static auto destroy(swapchain* handle) noexcept -> void;

        static auto query_support(swapchain_support_details* details, VkPhysicalDevice physical_device, VkSurfaceKHR surface) -> void;
    };
}

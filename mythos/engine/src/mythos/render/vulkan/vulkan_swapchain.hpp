#pragma once
#include <mythos/platform/window.hpp>
#include <mythos/render/vulkan/vulkan_context.hpp>

namespace myth::vulkan {
    struct swapchain_support_details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    class swapchain {
        context& m_context;
    public:
        static auto query_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, swapchain_support_details* details) -> void;

        MYL_NO_DISCARD swapchain(context& context, window& window);
        ~swapchain();
    };
}

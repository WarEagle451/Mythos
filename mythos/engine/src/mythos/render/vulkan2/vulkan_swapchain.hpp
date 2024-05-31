#pragma once
#include <mythos/render/vulkan2/vulkan_device.hpp>

namespace myth::vulkan2 {
    struct swapchain_support_details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> available_surface_formats;
        std::vector<VkPresentModeKHR> available_present_modes;
    };

    class swapchain {
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        VkExtent2D m_extent{ 0, 0 };
        VkSurfaceFormatKHR m_image_format{};

        uint32_t m_max_frames_in_flight{ 0 }; /// MYTODO: Should this be kept?

        std::vector<VkImage> m_images{};
        std::vector<VkImageView> m_views{};
        std::vector<VkFramebuffer> m_framebuffers;
    public:
        struct create_info {
            VkSurfaceKHR      surface;
            const VkExtent2D& extent;
        };
        
        static auto query_support(swapchain_support_details* details, VkPhysicalDevice physical_device, VkSurfaceKHR surface) -> void;

        static auto create(swapchain* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(swapchain* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        MYL_NO_DISCARD constexpr auto image_extent() const -> const VkExtent2D& { return m_extent; }
        MYL_NO_DISCARD constexpr auto image_format() const -> const VkSurfaceFormatKHR& { return m_image_format; }
        MYL_NO_DISCARD constexpr auto images() const -> const std::vector<VkImage>& { return m_images; }

        auto recreate_framebuffers(device& device, VkRenderPass render_pass, VkAllocationCallbacks* allocator) -> void;
        auto destroy_framebuffers(device& device, VkAllocationCallbacks* allocator) -> void;
    };
}

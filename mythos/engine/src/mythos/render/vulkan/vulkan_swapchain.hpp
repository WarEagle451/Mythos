#pragma once
#include <mythos/platform/window.hpp>
#include <mythos/render/vulkan/vulkan_render_pass.hpp>

#include <vector>

namespace myth::vulkan {
    struct swapchain_support_details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> present_modes;
    };

    class swapchain {
        context& m_context;

        VkSwapchainKHR m_swapchain;
        VkExtent2D m_extent;

        VkSurfaceFormatKHR m_surface_format;
        uint32_t m_current_image_index = 0;
        std::vector<VkImage> m_images;
        std::vector<VkImageView> m_views;

        std::vector<VkFramebuffer> m_framebuffers;
    public:
        static auto query_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, swapchain_support_details* details) -> void;

        MYL_NO_DISCARD swapchain(context& context, window& window);
        ~swapchain();

        MYL_NO_DISCARD constexpr auto extent() const -> const VkExtent2D& { return m_extent; }
        MYL_NO_DISCARD constexpr auto current_image_index() const -> uint32_t { return m_current_image_index; }
        MYL_NO_DISCARD constexpr auto images() const -> const std::vector<VkImage>& { return m_images; }
        MYL_NO_DISCARD constexpr auto image_format() const -> VkFormat { return m_surface_format.format; }
        MYL_NO_DISCARD constexpr auto framebuffers() const -> const std::vector<VkFramebuffer>& { return m_framebuffers; }

        auto recreate_framebuffers(render_pass& render_pass) -> void;
        auto destroy_framebuffers() -> void;
    private:
        auto create_swapchain(window& window) -> void;
        auto create_images_and_views() -> void;

        auto destroy_images_and_views() -> void;
        auto destroy_swapchain() -> void;
    };
}

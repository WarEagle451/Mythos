#pragma once
#include <mythos/render/vulkan/vulkan_device.hpp>

namespace myth::vulkan {
    struct swapchain_support_details {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> available_surface_formats;
        std::vector<VkPresentModeKHR> available_present_modes;
    };

    class swapchain {
        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        VkExtent2D m_extent{ 0, 0 };
        VkSurfaceFormatKHR m_image_format{};

        uint32_t m_max_frames_in_flight{ 0 };
        uint32_t m_current_image_index{ 0 };

        std::vector<VkImage> m_images{};
        std::vector<VkImageView> m_views{};
        std::vector<VkFramebuffer> m_framebuffers;
        std::vector<VkSemaphore> m_image_available_semaphores;
        std::vector<VkSemaphore> m_render_finished_semaphores;
        std::vector<VkFence> m_in_flight_fences;
    public:
        struct create_info {
            VkSurfaceKHR      surface;
            const VkExtent2D& extent;
            bool              vsync;
        };
        
        static auto query_support(swapchain_support_details* details, VkPhysicalDevice physical_device, VkSurfaceKHR surface) -> void;

        static auto create(swapchain* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(swapchain* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;
        static auto recreate(swapchain* handle, device& device, VkRenderPass render_pass, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;

        MYL_NO_DISCARD constexpr auto current_image_index() const noexcept -> uint32_t { return m_current_image_index; }
        MYL_NO_DISCARD constexpr auto max_frames_in_flight() const noexcept -> uint32_t { return m_max_frames_in_flight; }
        MYL_NO_DISCARD constexpr auto image_extent() const -> const VkExtent2D& { return m_extent; }
        MYL_NO_DISCARD constexpr auto image_format() const -> const VkSurfaceFormatKHR& { return m_image_format; }
        MYL_NO_DISCARD constexpr auto images() const -> const std::vector<VkImage>& { return m_images; }
        MYL_NO_DISCARD constexpr auto framebuffers() const -> const std::vector<VkFramebuffer>& { return m_framebuffers; }

        MYL_NO_DISCARD constexpr auto image_available_semaphores() -> std::vector<VkSemaphore>& { return m_image_available_semaphores; }
        MYL_NO_DISCARD constexpr auto render_finished_semaphores() -> std::vector<VkSemaphore>& { return m_render_finished_semaphores; }
        MYL_NO_DISCARD constexpr auto in_flight_fences() -> std::vector<VkFence>& { return m_in_flight_fences; }

        MYL_NO_DISCARD auto acquire_next_image(device& device, VkSemaphore semaphore) -> VkResult;
        MYL_NO_DISCARD auto present(VkQueue queue, const VkSemaphore* signal_semaphores) -> VkResult;

        auto recreate_framebuffers(device& device, VkRenderPass render_pass, VkAllocationCallbacks* allocator) -> void;
        auto destroy_framebuffers(device& device, VkAllocationCallbacks* allocator) -> void;
    };
}

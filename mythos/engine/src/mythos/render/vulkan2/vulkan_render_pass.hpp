#pragma once
#include <mythos/render/vulkan2/vulkan_device.hpp>

namespace myth::vulkan2 {
    class render_pass {
        VkRenderPass m_render_pass = VK_NULL_HANDLE;
        VkClearColorValue m_clear_color{};
    public:
        struct create_info {
            VkFormat          color_format;
            VkClearColorValue clear_color;
        };

        static auto create(render_pass* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(render_pass* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        MYL_NO_DISCARD constexpr auto handle() -> const VkRenderPass& { return m_render_pass; }

        auto begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer, const VkRect2D& render_area) -> void;
        auto end(VkCommandBuffer command_buffer) -> void;
    };
}

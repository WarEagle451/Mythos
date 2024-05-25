#pragma once
#include <mythos/render/vulkan/vulkan_context.hpp>

namespace myth::vulkan {
    class render_pass {
        context& m_context;

        VkRenderPass m_render_pass;
        VkClearColorValue m_clear_color;
    public:
        MYL_NO_DISCARD render_pass(context& context, VkFormat color_format);
        ~render_pass();

        MYL_NO_DISCARD constexpr auto handle() const -> VkRenderPass { return m_render_pass; }

        auto set_clear_color(const VkClearColorValue& color) -> void { m_clear_color = color; }

        auto begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer, const VkRect2D& render_area) -> void;
        auto end(VkCommandBuffer command_buffer) -> void;
    };
}

#pragma once
#include <mythos/render/vulkan/vulkan_context.hpp>

namespace myth::vulkan {
    class render_pass {
        context& m_context;

        VkRenderPass m_render_pass;
    public:
        MYL_NO_DISCARD render_pass(context& context, VkFormat color_format);
        ~render_pass();

        MYL_NO_DISCARD constexpr auto handle() const -> VkRenderPass { return m_render_pass; }
    };
}

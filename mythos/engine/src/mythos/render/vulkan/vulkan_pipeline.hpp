#pragma once
#include <mythos/render/vulkan/vulkan_context.hpp>

namespace myth::vulkan {
    class pipeline {
        context& m_context;

        VkPipeline m_pipeline;
        VkPipelineLayout m_layout;
    public:
        MYL_NO_DISCARD pipeline(
            context& context,
            VkRenderPass render_pass,
            const VkViewport& viewport,
            const VkRect2D& scissor,
            const VkPrimitiveTopology primitive,
            const std::vector<VkPipelineShaderStageCreateInfo>& shader_stage_create_infos);
        ~pipeline();

        auto bind(VkCommandBuffer command_buffer) -> void;
    };
}

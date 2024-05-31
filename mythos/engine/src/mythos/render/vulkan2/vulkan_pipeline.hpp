#pragma once
#include <mythos/render/vulkan2/vulkan_device.hpp>

namespace myth::vulkan2 {
    class pipeline {
        VkPipeline m_pipeline;
        VkPipelineLayout m_layout;
    public:
        struct create_info {
            const VkPrimitiveTopology                          primitive;
            const VkRect2D&                                    scissor;
            const VkViewport&                                  viewport;
            const std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
            VkRenderPass                                       render_pass;
        };

        static auto create(pipeline* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(pipeline* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        auto bind(VkCommandBuffer command_buffer) -> void;
    };
}

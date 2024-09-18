#pragma once
#include <mythos/render/shader.hpp>
#include <mythos/render/vulkan/vulkan_device.hpp>

namespace myth::vulkan {
    class pipeline {
        VkPipeline m_pipeline = VK_NULL_HANDLE;
        VkPipelineLayout m_layout = VK_NULL_HANDLE;
    public:
        struct create_info {
            const shader_layout&                               layout;
            const VkPrimitiveTopology                          primitive;
            const VkRect2D&                                    scissor;
            const VkViewport&                                  viewport;
            const std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;           
            VkRenderPass                                       render_pass;
            const std::vector<VkDescriptorSetLayout>           descriptor_set_layouts;
        };

        static auto create(pipeline* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(pipeline* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        auto bind(VkCommandBuffer command_buffer) -> void;
    };
}

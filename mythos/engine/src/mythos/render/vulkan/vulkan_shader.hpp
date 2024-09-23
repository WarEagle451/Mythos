#pragma once
#include <mythos/render/vulkan/vulkan_pipeline.hpp>
#include <mythos/render/vulkan/vulkan_swapchain.hpp>

#include <unordered_map>
#include <vector>

namespace myth::vulkan {
    class shader : public myth::shader {
        pipeline m_pipeline{};
    public:
        struct create_info {
            const VkExtent2D&                                          swapchain_extent;
            VkRenderPass                                               render_pass;
            const std::unordered_map<shader_type, shader_binary_type>& binaries;
            const shader_layout&                                       layout;
            const shader_primitive                                     primitive;
            const std::vector<VkDescriptorSetLayout>&                  descriptor_set_layouts;
        };

        static auto create(shader* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(shader* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        auto bind(VkCommandBuffer command_buffer, const VkDescriptorSet& descriptor_set) -> void;
        auto bind(VkCommandBuffer command_buffer, const std::vector<VkDescriptorSet>& descriptor_sets) -> void;
    };
}

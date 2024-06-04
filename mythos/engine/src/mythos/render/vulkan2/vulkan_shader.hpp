#pragma once
#include <mythos/render/shader.hpp>
#include <mythos/render/vulkan2/vulkan_pipeline.hpp>
#include <mythos/render/vulkan2/vulkan_swapchain.hpp>

namespace myth::vulkan2 {
    class shader : public myth::shader {
        pipeline m_pipeline{};
    public:
        struct create_info {
            const VkExtent2D&                                          swapchain_extent;
            VkRenderPass                                               render_pass;
            const std::unordered_map<shader_type, shader_binary_type>& binaries;
            const shader_primitive                                     primitive;
        };

        static auto create(shader* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(shader* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        auto bind(VkCommandBuffer command_buffer) -> void;
    };
}

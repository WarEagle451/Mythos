#pragma once
#include <mythos/render/vulkan/vulkan_device.hpp>

#include <vector>

namespace myth::vulkan {
    class descriptor_set {
        VkDescriptorSetLayout m_set_layout;
    public:
    };

    class descriptor_pool {
        VkDescriptorPool m_pool;
        VkDescriptorType m_descriptor_set_type;
    public:
        struct create_info {
            VkDescriptorType            descriptor_set_type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
            uint32_t                    descriptor_set_count;
            VkDescriptorPoolCreateFlags pool_create_flags;
        };

        static auto create(descriptor_pool* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(descriptor_pool* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        auto descriptor_type() const noexcept -> VkDescriptorType { return m_descriptor_set_type; };

        auto allocate(device& device, std::vector<VkDescriptorSet>* sets, const std::vector<VkDescriptorSetLayout>& layouts) -> void;
        auto deallocate(device& device, std::vector<VkDescriptorSet>* sets) -> void;
    };
}

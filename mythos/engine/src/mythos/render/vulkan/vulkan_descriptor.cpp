#include <mythos/render/vulkan/vulkan_descriptor.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace myth::vulkan {
    auto descriptor_pool::create(descriptor_pool* h, device& device, const create_info& ci, VkAllocationCallbacks* allocator) -> void {
        VkDescriptorPoolSize pool_size{
            .type            = ci.descriptor_set_type,
            .descriptorCount = ci.descriptor_set_count
        };
        
        VkDescriptorPoolCreateInfo descriptor_pool_create_info{
            .sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
            //.pNext         =,
            .flags         = ci.pool_create_flags,
            .maxSets       = ci.descriptor_set_count,
            .poolSizeCount = 1,
            .pPoolSizes    = &pool_size
        };

        MYTHOS_VULKAN_VERIFY(vkCreateDescriptorPool, device.logical(), &descriptor_pool_create_info, allocator, &h->m_pool);

        h->m_descriptor_set_type = ci.descriptor_set_type;
    }

    auto descriptor_pool::destroy(descriptor_pool* h, device& device, VkAllocationCallbacks* allocator) noexcept -> void {
        h->m_descriptor_set_type = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        vkDestroyDescriptorPool(device.logical(), h->m_pool, allocator);
    }

    auto descriptor_pool::allocate(device& device, std::vector<VkDescriptorSet>* sets, const std::vector<VkDescriptorSetLayout>& layouts) -> void {
        VkDescriptorSetAllocateInfo descriptor_set_allocate_info{
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            //.pNext              =,
            .descriptorPool = m_pool,
            .descriptorSetCount = static_cast<uint32_t>(layouts.size()),
            .pSetLayouts = layouts.data(),
        };

        sets->resize(layouts.size());
        MYTHOS_VULKAN_VERIFY(vkAllocateDescriptorSets, device.logical(), &descriptor_set_allocate_info, sets->data());
    }

    auto descriptor_pool::deallocate(device& device, std::vector<VkDescriptorSet>* sets) -> void {
        vkFreeDescriptorSets(device.logical(), m_pool, static_cast<uint32_t>(sets->size()), sets->data());
    }
}

#include <mythos/render/vulkan2/vulkan_command_objects.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace myth::vulkan2 {
    auto command_buffer::begin(VkCommandBufferUsageFlags flags) -> void {
        VkCommandBufferBeginInfo command_buffer_begin_info{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags            = flags,
            .pInheritanceInfo = nullptr
        };

        MYTHOS_VULKAN_VERIFY(vkBeginCommandBuffer, m_command_buffer, &command_buffer_begin_info);
    }

    auto command_buffer::end() -> void {
        MYTHOS_VULKAN_VERIFY(vkEndCommandBuffer, m_command_buffer);
    }

    auto command_buffer::reset(VkCommandBufferResetFlags flags) -> void {
        vkResetCommandBuffer(m_command_buffer, flags);
    }

    auto command_pool::create(command_pool* h, device& device, const create_info& ci, VkAllocationCallbacks* allocator) -> void {
        VkCommandPoolCreateInfo command_pool_create_info{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            //.pNext            = ,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = ci.queue_family_index
        };

        MYTHOS_VULKAN_VERIFY(vkCreateCommandPool, device.logical(), &command_pool_create_info, allocator, &h->m_command_pool);
    }

    auto command_pool::destroy(command_pool* h, device& device, VkAllocationCallbacks* allocator) noexcept -> void {
        if (h->m_command_pool)
            vkDestroyCommandPool(device.logical(), h->m_command_pool, allocator);
    }

    auto command_pool::allocate_command_buffers(device& device, std::vector<command_buffer>::iterator begin, std::vector<command_buffer>::iterator end) -> void {
        VkCommandBufferAllocateInfo command_buffer_allocate_info{
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            //.pNext = ,
            .commandPool        = m_command_pool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        while (begin != end) {
            if (begin->m_command_buffer != VK_NULL_HANDLE) {
                MYTHOS_ERROR("Double allocating command buffer '{}' is forbidden! Skipping...", reinterpret_cast<intptr_t>(begin->m_command_buffer));
                ++begin;
                continue;
            }

            MYTHOS_VULKAN_VERIFY(vkAllocateCommandBuffers, device.logical(), &command_buffer_allocate_info, &begin->m_command_buffer);
            ++begin;
        }
    }

    auto command_pool::deallocate_command_buffers(device& device, std::vector<command_buffer>::iterator begin, std::vector<command_buffer>::iterator end) -> void {
        while (begin != end) {
            if (begin->m_command_buffer == VK_NULL_HANDLE) {
                MYTHOS_ERROR("Double deallocating command buffer '{}' is forbidden! Skipping...", reinterpret_cast<intptr_t>(begin->m_command_buffer));
                ++begin;
                continue;
            }

            vkFreeCommandBuffers(device.logical(), m_command_pool, 1, &begin->m_command_buffer);
            ++begin;
        }
    }
}

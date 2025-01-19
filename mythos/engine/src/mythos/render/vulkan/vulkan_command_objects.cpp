#include <mythos/render/vulkan/vulkan_command_objects.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace mye::vulkan {
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

        h->m_queue = ci.queue;
    }

    auto command_pool::destroy(command_pool* h, device& device, VkAllocationCallbacks* allocator) noexcept -> void {
        h->m_queue = VK_NULL_HANDLE;

        if (h->m_command_pool)
            vkDestroyCommandPool(device.logical(), h->m_command_pool, allocator);
    }

    auto command_pool::allocate_command_buffers(device& device, command_buffer* first, myl::u32 count) -> void {
        VkCommandBufferAllocateInfo command_buffer_allocate_info{
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            //.pNext = ,
            .commandPool        = m_command_pool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        for (myl::u32 i = 0; i != count; ++i, ++first) {
            if (first->m_command_buffer != VK_NULL_HANDLE) {
                MYTHOS_ERROR("Command buffer '{}' is already allocated, allocating again is undefined behaviour, skipping...", reinterpret_cast<intptr_t>(first->m_command_buffer));
                continue;
            }

            MYTHOS_VULKAN_VERIFY(vkAllocateCommandBuffers, device.logical(), &command_buffer_allocate_info, &first->m_command_buffer);
        }
    }

    auto command_pool::allocate_command_buffers(device& device, std::vector<command_buffer>::iterator first, std::vector<command_buffer>::iterator last) -> void {
        VkCommandBufferAllocateInfo command_buffer_allocate_info{
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            //.pNext = ,
            .commandPool        = m_command_pool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        while (first != last) {
            if (first->m_command_buffer != VK_NULL_HANDLE) {
                MYTHOS_ERROR("Command buffer '{}' is already allocated, allocating again is undefined behaviour, skipping...", reinterpret_cast<intptr_t>(first->m_command_buffer));
                ++first;
                continue;
            }

            MYTHOS_VULKAN_VERIFY(vkAllocateCommandBuffers, device.logical(), &command_buffer_allocate_info, &first->m_command_buffer);
            ++first;
        }
    }

    auto command_pool::deallocate_command_buffers(device& device, command_buffer* first, myl::u32 count) -> void {
        for (myl::u32 i = 0; i != count; ++i, ++first) {
            if (first->m_command_buffer == VK_NULL_HANDLE) {
                MYTHOS_ERROR("Command buffer '{}' is already deallocated, deallocating again is undefined behaviour, skipping...", reinterpret_cast<intptr_t>(first->m_command_buffer));
                continue;
            }

            vkFreeCommandBuffers(device.logical(), m_command_pool, 1, &first->m_command_buffer);
        }
    }

    auto command_pool::deallocate_command_buffers(device& device, std::vector<command_buffer>::iterator first, std::vector<command_buffer>::iterator last) -> void {
        while (first != last) {
            if (first->m_command_buffer == VK_NULL_HANDLE) {
                MYTHOS_ERROR("Command buffer '{}' is already deallocated, deallocating again is undefined behaviour, skipping...", reinterpret_cast<intptr_t>(first->m_command_buffer));
                ++first;
                continue;
            }

            vkFreeCommandBuffers(device.logical(), m_command_pool, 1, &first->m_command_buffer);
            ++first;
        }
    }
}

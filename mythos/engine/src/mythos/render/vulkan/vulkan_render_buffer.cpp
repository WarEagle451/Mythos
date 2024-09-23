#include <mythos/render/vulkan/vulkan_render_buffer.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace myth::vulkan {
    MYL_NO_DISCARD constexpr auto calc_memory_offset(VkDeviceSize internal_block_offset, VkDeviceSize block_size, VkDeviceSize index) -> VkDeviceSize {
        return (block_size * index) + internal_block_offset;
    }

    auto render_buffer::create(render_buffer* handle, device& device, const create_info& ci, VkAllocationCallbacks* allocator) -> void {
        const VkDeviceSize total_buffer_size = ci.block_size * ci.block_count;
        
        VkBufferCreateInfo buffer_create_info{
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            //.pNext                 = ,
            //.flags                 = ,
            .size                  = total_buffer_size,
            .usage                 = ci.usage,
            .sharingMode           = ci.sharing_mode,
            //.queueFamilyIndexCount = ,
            //.pQueueFamilyIndices   = 
        };

        MYTHOS_VULKAN_VERIFY(vkCreateBuffer, device.logical(), &buffer_create_info, allocator, &handle->m_buffer);
        
        VkMemoryRequirements memory_requirements{};
        vkGetBufferMemoryRequirements(device.logical(), handle->m_buffer, &memory_requirements);

        VkMemoryAllocateInfo allocate_info{
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            //.pNext           = ,
            .allocationSize  = memory_requirements.size,
            .memoryTypeIndex = device.find_memory_type(memory_requirements.memoryTypeBits, ci.properties)
        };

        MYTHOS_VULKAN_VERIFY(vkAllocateMemory, device.logical(), &allocate_info, allocator, &handle->m_memory);
        handle->m_memory_block_size = ci.block_size;
        handle->m_memory_properties = ci.properties;
        handle->m_device = &device;
        handle->m_command_pool = &ci.command_pool;

        if (ci.persistent)
            vkMapMemory(device.logical(), handle->m_memory, 0, total_buffer_size, 0, &handle->persistent_mapping_ptr);

        handle->bind();
    }

    auto render_buffer::destroy(render_buffer* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void {
        // Constructor ensures both must be handled
        vkDestroyBuffer(device.logical(), handle->m_buffer, allocator);
        vkFreeMemory(device.logical(), handle->m_memory, allocator);
    }

    auto render_buffer::copy(render_buffer& src, render_buffer& dst, VkDeviceSize size, VkDeviceSize offset, VkDeviceSize index, command_pool& command_pool) -> void {
        command_buffer copy_command_buffer{};
        command_pool.allocate_command_buffers(*dst.m_device, &copy_command_buffer, 1);
        copy_command_buffer.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VkBufferCopy copy_region{
            .srcOffset = calc_memory_offset(offset, src.m_memory_block_size, index),
            .dstOffset = calc_memory_offset(offset, dst.m_memory_block_size, index),
            .size      = size,
        };

        vkCmdCopyBuffer(copy_command_buffer.handle(), src.handle(), dst.handle(), 1, &copy_region);
        copy_command_buffer.end();

        VkSubmitInfo submit_info{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            //.pNext                =,
            //.waitSemaphoreCount   =,
            //.pWaitSemaphores      =,
            //.pWaitDstStageMask    =,
            .commandBufferCount = 1,
            .pCommandBuffers = &copy_command_buffer.handle(),
            //.signalSemaphoreCount =,
            //.pSignalSemaphores    =,
        };

        /// MYTODO: Fences may also be used, this can allow the drive to optimize things
        vkQueueSubmit(command_pool.queue(), 1, &submit_info, VK_NULL_HANDLE);
        vkQueueWaitIdle(command_pool.queue());
        
        command_pool.deallocate_command_buffers(*dst.m_device, &copy_command_buffer, 1);
    }

    auto render_buffer::bind(myl::usize offset, myl::usize index) -> void {
        vkBindBufferMemory(m_device->logical(), m_buffer, m_memory, calc_memory_offset(static_cast<VkDeviceSize>(offset), m_memory_block_size, static_cast<VkDeviceSize>(index)));
    }

    auto render_buffer::unbind() -> void {

    }

    auto render_buffer::set(void* data, myl::usize size, myl::usize offset, myl::usize index) -> void {
        /// MYTODO: Look into "persistent mapping", the uniform buffer should be using this
        /// https://vulkan-tutorial.com/Uniform_buffers/Descriptor_layout_and_buffer "persistent mapping" in bold

        const VkDeviceSize true_offset = calc_memory_offset(static_cast<VkDeviceSize>(offset), m_memory_block_size, static_cast<VkDeviceSize>(index));
        if (persistent_mapping_ptr == nullptr) {
            void* data_ptr = nullptr;
            vkMapMemory(m_device->logical(), m_memory, true_offset, static_cast<VkDeviceSize>(size), 0, &data_ptr);
            memcpy(data_ptr, data, static_cast<size_t>(size));

            /// MYTODO: IMPLEMENT & Figure out where vkInvalidateMappedMemoryRanges should be
            /// https://vulkan-tutorial.com/en/Vertex_buffers/Vertex_buffer_creation - Above "Binding the vertex buffer"
            ///// Non VK_MEMORY_PROPERTY_HOST_COHERENT_BIT buffers must have mapped memory flushed
            ///if (!(m_memory_properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            ///    VkMappedMemoryRange range{
            ///        .sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
            ///        .memory = m_memory,
            ///        .offset = static_cast<VkDeviceSize>(offset),
            ///        .size   = static_cast<VkDeviceSize>(size)
            ///    };
            ///
            ///    vkFlushMappedMemoryRanges(m_logical_device, 1, &range);
            ///}

            vkUnmapMemory(m_device->logical(), m_memory);
        }
        else
            // Casting to enable pointer arithmetic
            memcpy(static_cast<myl::u8*>(persistent_mapping_ptr) + true_offset, data, static_cast<size_t>(size));
    }

    auto render_buffer::upload(void* data, myl::usize size, myl::usize offset, myl::usize index) -> void {
        const VkDeviceSize vk_size = static_cast<VkDeviceSize>(size);
        const VkDeviceSize vk_offset = static_cast<VkDeviceSize>(offset);
        const VkDeviceSize vk_index = static_cast<VkDeviceSize>(index);

        // The most optimal memory has the VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT flag and is usally not accessible by the CPU on dedicated graphics cards.
        // Therefore, a staging buffer must be used to set data.
        if (m_memory_properties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            /// MYTODO: dedicated command pool https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer
            ///     Should be on the transfer queue, but all graphics queues are transfer queues so this works for now
            create_info staging_buffer_create_info{
                .command_pool = *m_command_pool,
                .usage        = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                .properties   = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                .block_count  = 1,
                .block_size   = vk_size,
                .sharing_mode = VK_SHARING_MODE_EXCLUSIVE,
                .persistent   = false,
            };

            render_buffer staging_buffer{};
            render_buffer::create(&staging_buffer, *m_device, staging_buffer_create_info, VK_NULL_HANDLE); /// MYTODO: This is unable to pass an allocator into, fix

            staging_buffer.set(data, vk_size, vk_offset, vk_index);

            /// MYTODO: dedicated command pool https://vulkan-tutorial.com/Vertex_buffers/Staging_buffer
            ///     Should be on the transfer queue, but all graphics queues are transfer queues so this works for now
            /*
            * "Memory transfer operations are executed using command buffers, just like drawing commands.
            * Therefore we must first allocate a temporary command buffer. You may wish to create a separate
            * command pool for these kinds of short-lived buffers, because the implementation may be able to
            * apply memory allocation optimizations. You should use the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag
            * during command pool generation in that case."
            */
            render_buffer::copy(staging_buffer, *this, vk_size, vk_offset, vk_index, *m_command_pool); /// ARE THESE REVERSED???
            render_buffer::destroy(&staging_buffer, *m_device, VK_NULL_HANDLE); /// MYTODO: This is unable to pass an allocator into, fix
        }
        else
            set(data, size, offset, index);
    }
}

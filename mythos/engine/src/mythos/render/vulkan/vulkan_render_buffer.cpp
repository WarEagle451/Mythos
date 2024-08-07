#include <mythos/render/vulkan/vulkan_render_buffer.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace myth::vulkan {
    auto render_buffer::create(render_buffer* handle, device& device, const create_info& ci, VkAllocationCallbacks* allocator) -> void {
        VkBufferCreateInfo buffer_create_info{
            .sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
            ///.pNext                 = ,
            ///.flags                 = ,
            .size                  = ci.bytes,
            .usage                 = ci.usage,
            .sharingMode           = ci.sharing_mode,
            ///.queueFamilyIndexCount = ,
            ///.pQueueFamilyIndices   = 
        };

        MYTHOS_VULKAN_VERIFY(vkCreateBuffer, device.logical(), &buffer_create_info, allocator, &handle->m_buffer);

        /// MYTODO: MAYBE DO THIS IN BACKEND AND PASS INTO ci.properties, specifically -> VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        VkMemoryRequirements memory_requirements{};
        vkGetBufferMemoryRequirements(device.logical(), handle->m_buffer, &memory_requirements);

        VkMemoryAllocateInfo allocate_info{
            .sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
            ///.pNext           = ,
            .allocationSize  = memory_requirements.size,
            .memoryTypeIndex = device.find_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
        };

        MYTHOS_VULKAN_VERIFY(vkAllocateMemory, device.logical(), &allocate_info, allocator, &handle->m_memory);
        handle->m_logical_device = device.logical();
        handle->bind();
    }

    auto render_buffer::destroy(render_buffer* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void {
        // Constructor ensures both must be handled
        vkDestroyBuffer(device.logical(), handle->m_buffer, allocator);
        vkFreeMemory(device.logical(), handle->m_memory, allocator);
    }

    auto render_buffer::bind(myl::usize offset, myl::usize index) -> void {
        vkBindBufferMemory(m_logical_device, m_buffer, m_memory, offset); /// MYTODO: OFFSET NEEDS TO BE CALCULATED with index and blocksize
    }

    auto render_buffer::unbind() -> void {

    }

    auto render_buffer::set(void* data, myl::usize size, myl::usize offset, myl::usize index) -> void {
        void* data_ptr = nullptr;
        vkMapMemory(m_logical_device, m_memory, static_cast<VkDeviceSize>(offset), static_cast<VkDeviceSize>(size), 0, &data_ptr); /// MYTODO: OFFSET NEEDS TO BE CALCULATED with index and blocksize
        memcpy(data_ptr, data, static_cast<size_t>(size));
        
        /// MYTODO: IMPLEMENT & Figure out where vkInvalidateMappedMemoryRanges should be
        /// https://vulkan-tutorial.com/en/Vertex_buffers/Vertex_buffer_creation - Above "Binding the vertex buffer"
        ///// Non VK_MEMORY_PROPERTY_HOST_COHERENT_BIT buffers must have mapped memory flushed
        ///if (!(m_memory_flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
        ///    VkMappedMemoryRange range{
        ///        .sType  = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        ///        .memory = m_memory,
        ///        .offset = static_cast<VkDeviceSize>(offset),
        ///        .size   = static_cast<VkDeviceSize>(size)
        ///    };
        ///
        ///    vkFlushMappedMemoryRanges(m_logical_device, 1, &range);
        ///}

        vkUnmapMemory(m_logical_device, m_memory);
    }

    auto render_buffer::upload(void* data, myl::usize size, myl::usize offset, myl::usize index) -> void {

    }
}

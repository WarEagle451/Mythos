#pragma once
#include <mythos/render/render_buffer.hpp>

#include <mythos/render/vulkan/vulkan_device.hpp>

namespace mythos::vulkan {
    class render_buffer {
        VkBuffer     m_buffer = VK_NULL_HANDLE;
        VkDeviceSize m_block_size = 0;
        ///VkDeviceMemory m_memory;
        ///VkMemoryPropertyFlags m_memory_flags;
    public:
        struct create_info {
            VkDeviceSize          size;
            VkBufferUsageFlags    usage;
            VkMemoryPropertyFlags properties;
            VkDeviceSize          block_count = 1;
            VkDeviceSize          min_offset_alignment = 1; /// MYBUG: Should this be 1?
        };

        static auto create(render_buffer* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(render_buffer* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        auto bind(myl::usize offset = 0, myl::usize index = 0) -> void override;
        auto unbind() -> void override;

        /// MYTODO: WHAT IS THE DIFFERENCE BETWEEN UPLOAD AND SET?
        auto upload(void* data, myl::usize size, myl::usize offset = 0, myl::usize index = 0) -> void override;
        ///auto set(void* data, VkDeviceSize size, VkDeviceSize offset = 0, VkDeviceSize index = 0) -> void;
        ///auto copy_to(vulkan::render_buffer* to, VkDeviceSize size, VkDeviceSize index = 0, VkDeviceSize dst_index = 0) -> void;
    };
}

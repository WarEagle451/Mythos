#pragma once
#include <mythos/render/render_buffer.hpp>
#include <mythos/render/vulkan/vulkan_device.hpp>

namespace myth::vulkan {
    MYL_NO_DISCARD static constexpr auto render_buffer_usage_to_VkBufferUsageFlags(render_buffer_usage usage) -> VkBufferUsageFlags {
        switch (usage) {
            using enum render_buffer_usage;
        case index:  return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case vertex: return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        default:     return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
        }
    }

    class render_buffer : public myth::render_buffer {
        VkBuffer       m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
        VkDevice       m_logical_device = VK_NULL_HANDLE; // Device may needed following creation

        ///VkMemoryPropertyFlags m_memory_flags;
    public:
        struct create_info {
            VkBufferUsageFlags    usage;
            VkDeviceSize          bytes;
            /// MYTODO: PUT INTO USE
            ///VkMemoryPropertyFlags properties;
            VkSharingMode         sharing_mode;
        };

        static auto create(render_buffer* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(render_buffer* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        MYL_NO_DISCARD auto handle() -> VkBuffer& { return m_buffer; }

        auto bind(myl::usize offset = 0, myl::usize index = 0) -> void override;
        auto unbind() -> void override;

        /// MYTODO: WHAT IS THE DIFFERENCE BETWEEN UPLOAD AND SET?
        auto set(void* data, myl::usize size, myl::usize offset = 0, myl::usize index = 0) -> void override;
        auto upload(void* data, myl::usize size, myl::usize offset = 0, myl::usize index = 0) -> void override;
        ///auto copy_to(vulkan::render_buffer* to, VkDeviceSize size, VkDeviceSize index = 0, VkDeviceSize dst_index = 0) -> void;
    };
}

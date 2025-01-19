#pragma once
#include <mythos/render/render_buffer.hpp>
#include <mythos/render/vulkan/vulkan_command_objects.hpp>
#include <mythos/render/vulkan/vulkan_device.hpp>

namespace mye::vulkan {
    MYL_NO_DISCARD static constexpr auto render_buffer_usage_to_VkBufferUsageFlags(render_buffer_usage usage) -> VkBufferUsageFlags {
        switch (usage) {
            using enum render_buffer_usage;
        case index:    return VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        case uniform:  return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case vertex:   return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        default:       return VK_BUFFER_USAGE_FLAG_BITS_MAX_ENUM;
        }
    }

    class render_buffer : public mye::render_buffer {
        /// MYTODO: Best to not have pointers of device and command_pool, remove if possible

        VkBuffer              m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory        m_memory = VK_NULL_HANDLE;
        VkDeviceSize          m_memory_block_size = 0;
        VkMemoryPropertyFlags m_memory_properties;
        device*               m_device = nullptr;
        command_pool*         m_command_pool = nullptr;
        void*              persistent_mapping_ptr = nullptr;
    public:
        struct create_info {
            command_pool&         command_pool;
            VkBufferUsageFlags    usage;
            VkMemoryPropertyFlags properties;
            VkDeviceSize          block_count;
            VkDeviceSize          block_size;
            VkSharingMode         sharing_mode;
            bool                  persistent;
        };

        static auto create(render_buffer* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(render_buffer* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        static auto copy(render_buffer&, render_buffer&, VkDeviceSize size, VkDeviceSize offset, VkDeviceSize index, command_pool& command_pool) -> void;

        MYL_NO_DISCARD auto handle() -> VkBuffer& { return m_buffer; }

        auto bind(myl::usize offset = 0, myl::usize index = 0) -> void override;
        auto unbind() -> void override;

        auto set(void* data, myl::usize size, myl::usize offset = 0, myl::usize index = 0) -> void override;
        auto upload(void* data, myl::usize size, myl::usize offset = 0, myl::usize index = 0) -> void override;
    };
}

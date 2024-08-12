#pragma once
#include <mythos/render/vulkan/vulkan_device.hpp>

#include <vector>

namespace myth::vulkan {
    class command_pool; // Forward declaration

    class command_buffer {
        friend command_pool;

        VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
    public:
        MYL_NO_DISCARD constexpr auto handle() -> const VkCommandBuffer& { return m_command_buffer; }

        auto begin(VkCommandBufferUsageFlags flags = 0) -> void;
        auto end() -> void;
        auto reset(VkCommandBufferResetFlags flags = 0) -> void;
    };

    class command_pool {
        VkCommandPool m_command_pool = VK_NULL_HANDLE;
        VkQueue       m_queue = VK_NULL_HANDLE; // non-owning pointer to the queue the command pool is for
    public:
        struct create_info {
            VkQueue  queue;
            uint32_t queue_family_index;
        };

        static auto create(command_pool* handle, device& device, const create_info& create_info, VkAllocationCallbacks* allocator) -> void;
        static auto destroy(command_pool* handle, device& device, VkAllocationCallbacks* allocator) noexcept -> void;

        auto queue() const -> VkQueue { return m_queue; };

        auto allocate_command_buffers(device& device, command_buffer* first, myl::u32 count) -> void;
        auto allocate_command_buffers(device& device, std::vector<command_buffer>::iterator first, std::vector<command_buffer>::iterator last) -> void;

        auto deallocate_command_buffers(device& device, command_buffer* first, myl::u32 count) -> void;
        auto deallocate_command_buffers(device& device, std::vector<command_buffer>::iterator first, std::vector<command_buffer>::iterator last) -> void;
    };
}

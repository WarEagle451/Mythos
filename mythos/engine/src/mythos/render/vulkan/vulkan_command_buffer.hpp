#pragma once
#include <myl/definitions.hpp>

#include <vulkan/vulkan.h>

namespace myth::vulkan {
    class context; // Forward declaration

    class command_buffer {
        context& m_context;

        VkCommandBuffer m_buffer = VK_NULL_HANDLE;
    public:
        MYL_NO_DISCARD command_buffer(context& context);
        ~command_buffer();

        auto handle() const -> VkCommandBuffer { return m_buffer; }

        auto allocate() -> void;
        auto deallocate() -> void;

        auto begin(VkCommandBufferUsageFlags flags = 0) -> void;
        auto end() -> void;

        auto reset() -> void;
    };
}

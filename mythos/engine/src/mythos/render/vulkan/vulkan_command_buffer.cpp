#include <mythos/render/vulkan/vulkan_command_buffer.hpp>
#include <mythos/render/vulkan/vulkan_context.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace myth::vulkan {
    MYL_NO_DISCARD command_buffer::command_buffer(context& context) :
        m_context{ context } {

    }

    command_buffer::~command_buffer() {
        if (m_buffer != VK_NULL_HANDLE)
            deallocate();
    }

    auto command_buffer::allocate() -> void {
        VkCommandBufferAllocateInfo command_buffer_allocate_info{
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            //.pNext = ,
            .commandPool        = m_context.command_pool(),
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        MYTHOS_VULKAN_VERIFY(vkAllocateCommandBuffers, m_context.device(), &command_buffer_allocate_info, &m_buffer);
    }

    auto command_buffer::deallocate() -> void {
        vkFreeCommandBuffers(m_context.device(), m_context.command_pool(), 1, &m_buffer);
        m_buffer = VK_NULL_HANDLE;
    }

    auto command_buffer::begin(VkCommandBufferUsageFlags flags) -> void {
        VkCommandBufferBeginInfo command_buffer_begin_info{
            .sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags            = flags,
            .pInheritanceInfo = nullptr
        };

        MYTHOS_VULKAN_VERIFY(vkBeginCommandBuffer, m_buffer, &command_buffer_begin_info);
    }

    auto command_buffer::end() -> void {
        MYTHOS_VULKAN_VERIFY(vkEndCommandBuffer, m_buffer);
    }

    auto command_buffer::reset() -> void {
        vkResetCommandBuffer(m_buffer, 0);
    }
}

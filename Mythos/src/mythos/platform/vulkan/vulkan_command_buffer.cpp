#include "vulkan_command_buffer.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"

namespace myl::vulkan {
	void command_buffer::allocate_and_begin_single_use(command_buffer& a_command_buffer, VkCommandPool a_pool) {
		a_command_buffer.allocate(a_pool, true);
		a_command_buffer.begin(true, false, false);
	}

	void command_buffer::dellocate_and_end_single_use(command_buffer& a_command_buffer, VkCommandPool a_pool, VkQueue a_queue) {
		a_command_buffer.end();

		VkSubmitInfo submit_info{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.commandBufferCount = 1,
			.pCommandBuffers = &a_command_buffer.m_handle,
		};

		MYL_VK_ASSERT(vkQueueSubmit, a_queue, 1, &submit_info, VK_NULL_HANDLE);
		MYL_VK_ASSERT(vkQueueWaitIdle, a_queue); // Wait for it to finish

		a_command_buffer.deallocate(a_pool);
	}

	command_buffer::command_buffer(context& a_context)
		: m_context(a_context)
		, m_state(command_buffer_state::not_allocated) {

	}

	command_buffer::~command_buffer() {
		/// MYTodo: What to do if its recording or allocated
	}

	void command_buffer::allocate(VkCommandPool a_pool, bool a_is_primary) {
		VkCommandBufferAllocateInfo alloc_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = 0,
			.commandPool = a_pool,
			.level = a_is_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
			.commandBufferCount = 1,
		};

		m_state = command_buffer_state::not_allocated;
		MYL_VK_ASSERT(vkAllocateCommandBuffers, m_context.m_device, &alloc_info, &m_handle);
		m_state = command_buffer_state::ready;
	}

	void command_buffer::deallocate(VkCommandPool a_pool) {
		vkFreeCommandBuffers(m_context.m_device, a_pool, 1, &m_handle);

		m_handle = VK_NULL_HANDLE;
		m_state = command_buffer_state::not_allocated;
	}

	void command_buffer::begin(bool a_is_single_use, bool a_is_render_pass_continue, bool a_is_simultaneous_use) {
		VkCommandBufferBeginInfo begin_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0
		};
		if (a_is_single_use)
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (a_is_render_pass_continue)
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		if (a_is_simultaneous_use)
			begin_info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		MYL_VK_ASSERT(vkBeginCommandBuffer, m_handle, &begin_info);
		m_state = command_buffer_state::recording;
	}

	void command_buffer::end() {
		MYL_VK_ASSERT(vkEndCommandBuffer, m_handle);
		m_state = command_buffer_state::recording_ended;
	}

	void command_buffer::update_submitted() {
		m_state = command_buffer_state::submitted;
	}

	void command_buffer::reset() {
		m_state = command_buffer_state::ready;
	}
}
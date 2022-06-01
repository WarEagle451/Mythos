#include "vulkan_command_buffer.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"

namespace myl::vulkane {
	void command_buffer::allocate_and_begin_single_use(command_buffer& a_command_buffer) {
		a_command_buffer.allocate(true);
		a_command_buffer.begin(true, false, false);
	}

	void command_buffer::deallocate_and_end_single_use(command_buffer& a_command_buffer, VkQueue a_queue) {
		a_command_buffer.end();

		VkSubmitInfo info{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.pCommandBuffers = &a_command_buffer.m_handle
		};

		MYL_VK_CHECK(vkQueueSubmit, a_queue, 1, &info, 0);

		// Wait for it to finish, instead of using a fence
		MYL_VK_CHECK(vkQueueWaitIdle, a_queue);

		a_command_buffer.deallocate();
	}

	command_buffer::command_buffer(context& a_context, VkCommandPool a_command_pool) 
		: m_context(a_context)
		, m_pool(a_command_pool)
		, m_state(command_buffer_state::not_allocated)
		, m_handle(VK_NULL_HANDLE) {}

	command_buffer::~command_buffer() {
		if (m_state != command_buffer_state::not_allocated)
			deallocate();
	}

	void command_buffer::begin(bool a_single_use, bool a_render_pass_continue, bool a_simultaneous_use) {
		VkCommandBufferBeginInfo info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0
		};

		if (a_single_use)
			info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (a_render_pass_continue)
			info.flags |= VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		if (a_simultaneous_use)
			info.flags |= VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		MYL_VK_CHECK(vkBeginCommandBuffer, m_handle, &info);
		m_state = command_buffer_state::recording;
	}

	void command_buffer::end() {
		MYL_VK_CHECK(vkEndCommandBuffer, m_handle);
		m_state = command_buffer_state::recording_ended;
	}

	void command_buffer::allocate(bool a_primary) {
		VkCommandBufferAllocateInfo alloc_info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = m_pool,
			.level = a_primary ? VK_COMMAND_BUFFER_LEVEL_PRIMARY : VK_COMMAND_BUFFER_LEVEL_SECONDARY,
			.commandBufferCount = 1
		};

		MYL_VK_CHECK(vkAllocateCommandBuffers, m_context.device().logical(), &alloc_info, &m_handle);
		m_state = command_buffer_state::ready;
	}

	void command_buffer::deallocate() {
		vkFreeCommandBuffers(m_context.device().logical(), m_pool, 1, &m_handle);
		m_handle = VK_NULL_HANDLE;
		m_state = command_buffer_state::not_allocated;
	}

	void command_buffer::update_submitted() {
		m_state = command_buffer_state::submitted;
	}

	void command_buffer::reset() {
		m_state = command_buffer_state::ready;
	}
}
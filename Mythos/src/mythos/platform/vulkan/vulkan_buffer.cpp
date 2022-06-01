#include "vulkan_buffer.hpp"
#include "vulkan_context.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_utils.hpp"

/// MYTodo: Go over everything again and compare it to first attempt


namespace myl::vulkan {
	buffer::buffer(context& a_context, u64 a_size, VkBufferUsageFlagBits a_usage, u32 a_memory_property_flags, bool a_bind_on_create)
		: m_context(a_context)
		, m_usage(a_usage)
		, m_size(a_size)
		, m_memory_property_flags(a_memory_property_flags) {

		VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.size = m_size;
		buffer_info.usage = m_usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.

		MYL_VK_ASSERT(vkCreateBuffer, m_context.device(), &buffer_info, VK_NULL_HANDLE, &m_handle);

		// Gather memory requirements.
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(m_context.device(), m_handle, &requirements);
		m_memory_index = m_context.find_memory_index(requirements.memoryTypeBits, m_memory_property_flags);
		if (m_memory_index == -1)
			throw vulkan_error("Failed to create buffer: Required memory type index not found");

		// Allocate memory info
		VkMemoryAllocateInfo allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocate_info.allocationSize = requirements.size;
		allocate_info.memoryTypeIndex = static_cast<u32>(m_memory_index);

		// Allocate the memory.
		VkResult result = vkAllocateMemory(
			m_context.device(),
			&allocate_info,
			VK_NULL_HANDLE,
			&m_memory);

		if (result != VK_SUCCESS)
			throw vulkan_error("'vkAllocateMemory' failed");

		if (a_bind_on_create)
			bind(0);
	}

	buffer::~buffer() {
		if (m_memory) {
			vkFreeMemory(m_context.device(), m_memory, VK_NULL_HANDLE);
			m_memory = 0;
		}
		if (m_handle) {
			vkDestroyBuffer(m_context.device(), m_handle, VK_NULL_HANDLE);
			m_handle = 0;
		}
		m_size = 0;
		m_usage = static_cast<VkBufferUsageFlagBits>(0);
		m_locked = false;
	}

	void buffer::resize(u64 a_size, VkQueue a_queue, VkCommandPool a_pool) {
		// Create new buffer.
		VkBufferCreateInfo buffer_info = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_info.size = a_size;
		buffer_info.usage = m_usage;
		buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;  // NOTE: Only used in one queue.

		VkBuffer new_buffer;
		MYL_VK_ASSERT(vkCreateBuffer, m_context.device(), &buffer_info, VK_NULL_HANDLE, &new_buffer);

		// Gather memory requirements.
		VkMemoryRequirements requirements;
		vkGetBufferMemoryRequirements(m_context.device(), new_buffer, &requirements);

		// Allocate memory info
		VkMemoryAllocateInfo allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		allocate_info.allocationSize = requirements.size;
		allocate_info.memoryTypeIndex = (u32)m_memory_index;

		// Allocate the memory.
		VkDeviceMemory new_memory;
		VkResult result = vkAllocateMemory(m_context.device(), &allocate_info, VK_NULL_HANDLE, &new_memory); /// MYTodo: should have a function that does this
		if (result != VK_SUCCESS)
			throw vulkan_error("'vkAllocateMemory' failed");

		// Bind the new buffer's memory
		MYL_VK_ASSERT(vkBindBufferMemory, m_context.device(), new_buffer, new_memory, 0);

		// Copy over the data
		copy_to(new_buffer, a_pool, 0, a_queue, 0, m_size);

		// Make sure anything potentially using these is finished.
		vkDeviceWaitIdle(m_context.device());

		// Destroy the old
		if (m_memory) {
			vkFreeMemory(m_context.device(), m_memory, VK_NULL_HANDLE);
			m_memory = 0;
		}
		if (m_handle) {
			vkDestroyBuffer(m_context.device(), m_handle, VK_NULL_HANDLE);
			m_handle = 0;
		}

		// Set new properties
		m_size = a_size;
		m_memory = new_memory;
		m_handle = new_buffer;
	}

	void buffer::bind(u64 a_offset) {
		MYL_VK_ASSERT(vkBindBufferMemory, m_context.device(), m_handle, m_memory, a_offset);
	}

	void* buffer::lock(u64 a_offset, u64 a_size, u32 a_flags) {
		void* data;
		MYL_VK_ASSERT(vkMapMemory, m_context.device(), m_memory, a_offset, a_size, a_flags, &data);
		return data;
	}

	void buffer::unlock() {
		vkUnmapMemory(m_context.device(), m_memory);
	}

	void buffer::load(u64 a_offset, u64 a_size, u32 a_flags, const void* a_data) {
		void* data_ptr;
		MYL_VK_ASSERT(vkMapMemory, m_context.device(), m_memory, a_offset, a_size, a_flags, &data_ptr);
		memcpy(data_ptr, a_data, a_size); /// MYTodo: Temp
		vkUnmapMemory(m_context.device(), m_memory);
	}

	void buffer::copy_to(VkBuffer a_buffer, VkCommandPool a_pool, VkFence a_fence, VkQueue a_queue, u64 a_offset, u64 a_size) {
		vkQueueWaitIdle(a_queue);
		// Create a one-time-use command buffer.
		command_buffer temp_command_buffer(m_context);
		command_buffer::allocate_and_begin_single_use(temp_command_buffer, a_pool);

		// Prepare the copy command and add it to the command buffer.
		VkBufferCopy copy_region;
		copy_region.srcOffset = 0;
		copy_region.dstOffset = a_offset;
		copy_region.size = a_size;

		vkCmdCopyBuffer(temp_command_buffer.handle(), m_handle, a_buffer, 1, &copy_region);

		// Submit the buffer for execution and wait for it to complete.
		command_buffer::dellocate_and_end_single_use(temp_command_buffer, a_pool, a_queue);
	}
}
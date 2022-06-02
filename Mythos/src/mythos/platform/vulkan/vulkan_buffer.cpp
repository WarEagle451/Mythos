#include "vulkan_buffer.hpp"
#include "vulkan_context.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_utils.hpp"

namespace myl::vulkan {
	buffer::buffer(context& a_context, u64 a_size, VkBufferUsageFlags a_usage, u32 a_memory_property_flags, bool a_bind_on_create)
		: m_context(a_context)
		, m_usage(a_usage)
		, m_size(a_size)
		, m_memory_property_flags(a_memory_property_flags)
		, m_locked(false) {
		VkBufferCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = m_size,
			.usage = m_usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE // VK_SHARING_MODE_EXCLUSIVE = Only used in one queue
		};

		MYL_VK_ASSERT(vkCreateBuffer, m_context.device(), &create_info, VK_NULL_HANDLE, &m_handle);

		// Gather memory requirements.
		VkMemoryRequirements requirements{};
		vkGetBufferMemoryRequirements(m_context.device(), m_handle, &requirements);

		m_memory_index = m_context.find_memory_index(requirements.memoryTypeBits, m_memory_property_flags);
		if (m_memory_index == ~0)
			MYL_CORE_ERROR("Failed to create buffer: Required memory type index not found");

		VkMemoryAllocateInfo allocate_info{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = requirements.size,
			.memoryTypeIndex = m_memory_index
		};

		MYL_VK_VERIFY(vkAllocateMemory, m_context.device(), &allocate_info, VK_NULL_HANDLE, &m_memory);

		if (a_bind_on_create)
			bind(0);
	}

	buffer::~buffer() {
		if (m_memory) vkFreeMemory(m_context.device(), m_memory, VK_NULL_HANDLE);
		if (m_handle) vkDestroyBuffer(m_context.device(), m_handle, VK_NULL_HANDLE);
	}

	void buffer::resize(u64 a_size, VkQueue a_queue, VkCommandPool a_pool) {
		VkBufferCreateInfo buffer_info{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.size = a_size,
			.usage = m_usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE // VK_SHARING_MODE_EXCLUSIVE = Only used in one queue
		};

		VkBuffer new_buffer{};
		MYL_VK_ASSERT(vkCreateBuffer, m_context.device(), &buffer_info, VK_NULL_HANDLE, &new_buffer);

		VkMemoryRequirements requirements{};
		vkGetBufferMemoryRequirements(m_context.device(), new_buffer, &requirements);

		VkMemoryAllocateInfo allocate_info{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = requirements.size,
			.memoryTypeIndex = m_memory_index
		};

		VkDeviceMemory new_memory{};
		MYL_VK_VERIFY(vkAllocateMemory, m_context.device(), &allocate_info, VK_NULL_HANDLE, &new_memory); /// MYTodo: should have a function that does this
		MYL_VK_ASSERT(vkBindBufferMemory, m_context.device(), new_buffer, new_memory, 0);

		copy_to(new_buffer, a_pool, 0, a_queue, 0, m_size);

		vkDeviceWaitIdle(m_context.device());

		if (m_memory) vkFreeMemory(m_context.device(), m_memory, VK_NULL_HANDLE);
		if (m_handle) vkDestroyBuffer(m_context.device(), m_handle, VK_NULL_HANDLE);

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
		m_locked = true;
		return data;
	}

	void buffer::unlock() {
		vkUnmapMemory(m_context.device(), m_memory);
		m_locked = false;
	}

	void buffer::load(u64 a_offset, u64 a_size, u32 a_flags, const void* a_data) {
		void* data_ptr;
		MYL_VK_ASSERT(vkMapMemory, m_context.device(), m_memory, a_offset, a_size, a_flags, &data_ptr);
		memcpy(data_ptr, a_data, a_size); /// MYTodo: Temp
		vkUnmapMemory(m_context.device(), m_memory);
	}

	void buffer::copy_to(VkBuffer a_buffer, VkCommandPool a_pool, VkFence a_fence, VkQueue a_queue, u64 a_offset, u64 a_size) {
		vkQueueWaitIdle(a_queue);

		command_buffer temp_command_buffer(m_context);
		command_buffer::allocate_and_begin_single_use(temp_command_buffer, a_pool);

		VkBufferCopy copy_region{
			.srcOffset = 0,
			.dstOffset = a_offset,
			.size = a_size
		};

		vkCmdCopyBuffer(temp_command_buffer.handle(), m_handle, a_buffer, 1, &copy_region);

		command_buffer::dellocate_and_end_single_use(temp_command_buffer, a_pool, a_queue);
	}
}
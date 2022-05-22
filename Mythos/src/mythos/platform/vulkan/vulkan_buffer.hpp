#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

/// MYTodo: should this just be a buffer class with a usage flag, and could move context's create buffer into constructor

namespace myl::vulkan {
	class context; // fwd declaration

	class buffer {
		context& m_context;

		VkBuffer m_handle;
		VkBufferUsageFlags m_usage;
		u64 m_size;
		VkDeviceMemory m_memory;
		bool m_locked;
		u32 m_memory_index;
		u32 m_memory_property_flags;
	public:
		buffer(context&, VkDeviceSize, VkBufferUsageFlags, u32 a_memory_property_flags, bool a_bind_on_create);
		~buffer();

		buffer(const buffer&) = delete;
		buffer& operator=(const buffer&) = delete;

		void resize(u64 a_size, VkQueue, VkCommandPool);

		void bind(u64 a_offset);
		void* lock(u64 a_offset, u64 a_size, u32 a_flags);
		void unlock();

		void load(u64 a_offset, u64 a_size, u32 a_flags, void* a_data);
		void copy_to(VkCommandPool, VkFence, VkQueue, VkBuffer, u64 a_offset, u64 a_size);
	};
}
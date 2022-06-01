#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

namespace myl::vulkane {
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

		VkBuffer& handle() { return m_handle; }

		void resize(u64 a_size, VkQueue, VkCommandPool);

		void bind(u64 a_offset);
		void* lock(u64 a_offset, u64 a_size, u32 a_flags);
		void unlock();

		void load(u64 a_offset, u64 a_size, u32 a_flags, const void* a_data);
		void copy_to(VkCommandPool, VkFence, VkQueue, VkBuffer&, u64 a_offset, u64 a_size); /// MYTodo: copy_to should take buffer first
	};
}
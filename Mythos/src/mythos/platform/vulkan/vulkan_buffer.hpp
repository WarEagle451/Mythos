#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

namespace myl::vulkan {
	class context; // fwd declaration

	class buffer {
		context& m_context;

		VkBuffer m_handle;
		VkBufferUsageFlags m_usage;
		u64 m_size;
		VkDeviceMemory m_memory;
		u32 m_memory_index;
		u32 m_memory_property_flags;
		bool m_locked;
	public:
		buffer(context&, u64 a_size, VkBufferUsageFlags, u32 a_memory_property_flags, bool a_bind_on_create);
		~buffer();

		buffer(const buffer&) = delete;
		buffer& operator=(const buffer&) = delete;

		VkBuffer& handle() { return m_handle; }

		void resize(u64 a_size, VkQueue, VkCommandPool);

		void bind(u64 a_offset);
		void* lock(u64 a_offset, u64 a_size, u32 a_flags);
		void unlock();

		void load(u64 a_offset, u64 a_size, u32 a_flags, const void* a_data);
		void copy_to(VkBuffer, VkCommandPool, VkFence, VkQueue, u64 a_offset, u64 a_size);
	};
}
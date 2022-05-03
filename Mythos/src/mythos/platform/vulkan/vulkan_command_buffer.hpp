#pragma once
#include <vulkan/vulkan.h>

namespace myl::vulkan {
	enum class command_buffer_state {
		ready,
		recording,
		in_render_pass,
		recording_ended,
		submitted,
		not_allocated
	};

	class command_buffer {
		VkCommandBuffer m_handle;

		command_buffer_state m_state;
	public:
		VkCommandBuffer& handle() { return m_handle; }

		void set_state(command_buffer_state a_state) { m_state = a_state; }
	};
}
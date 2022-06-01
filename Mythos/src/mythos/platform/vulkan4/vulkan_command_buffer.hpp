#pragma once
#include <vulkan/vulkan.h>

namespace myl::vulkan4 {
	class context; // fwd declaration

	enum class command_buffer_state {
		ready,
		recording,
		in_render_pass,
		recording_ended,
		submitted,
		not_allocated
	};

	class command_buffer {
		context& m_context;

		VkCommandBuffer m_handle;
		command_buffer_state m_state;
	public:
		static void allocate_and_begin_single_use(command_buffer&, VkCommandPool);
		static void dellocate_and_end_single_use(command_buffer&, VkCommandPool, VkQueue);

		command_buffer(context&);
		~command_buffer();

		VkCommandBuffer& handle() { return m_handle; }

		void set_state(command_buffer_state a_state) { m_state = a_state; }

		void allocate(VkCommandPool, bool a_is_primary);
		void deallocate(VkCommandPool);

		void begin(bool a_is_single_use, bool a_is_render_pass_continue, bool a_is_simultaneous_use);
		void end();

		void update_submitted();
		void reset();
	};
}
#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

namespace myl::vulkan {
	class context; // fdw declaration

	enum class command_buffer_state {
		ready,
		recording,
		in_render_pass,
		recording_ended,
		submitted,
		not_allocated
	};

	class command_buffer {
		context& m_context; // context must outlive command_buffer
		VkCommandPool m_pool;

		VkCommandBuffer m_handle;

		command_buffer_state m_state;
	public:
		static void allocate_and_begin_single_use(command_buffer&);
		static void deallocate_and_end_single_use(command_buffer&, VkQueue);

		command_buffer(context&, VkCommandPool);
		~command_buffer();

		MYL_NO_DISCARD VkCommandBuffer& handle() { return m_handle; }

		void set_state(command_buffer_state a_state) { m_state = a_state; }

		void begin(bool a_single_use, bool a_render_pass_continue, bool a_simultaneous_use);
		void end();

		void allocate(bool a_primary);
		void deallocate();

		//@brief update submitted states
		void update_submitted();
		void reset();
	};
}
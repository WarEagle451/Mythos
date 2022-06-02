#pragma once
#include "vulkan_command_buffer.hpp"

#include <mythos/math/vec4.hpp>

namespace myl::vulkan {
	class context; // fwd declaration
	class swapchain; // fwd declaration

	enum class render_pass_state {
		ready,
		recording,
		in_render_pass,
		recording_ended,
		submitted,
		not_allocated
	};

	class render_pass {
		context& m_context;

		VkRenderPass m_handle;

		f32 m_x, m_y;
		VkExtent2D m_extent;
		f32vec4 m_color;

		f32 m_depth;
		u32 m_stencil;

		render_pass_state m_state;
	public:
		render_pass(context&, swapchain&, const f32vec4& a_color, f32 a_x, f32 a_y, const VkExtent2D&, f32 a_depth, u32 a_stencil);
		~render_pass();

		VkRenderPass& handle() { return m_handle; }
		VkExtent2D& extent() { return m_extent; }
		f32& x() { return m_x; } /// MYTodo: Rid
		f32& y() { return m_y; } /// MYTodo: Rid

		void begin(command_buffer*, VkFramebuffer);
		void end(command_buffer*);
	};
}
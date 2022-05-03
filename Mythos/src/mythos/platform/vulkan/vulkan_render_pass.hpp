#pragma once
#include "vulkan_command_buffer.hpp"

#include <mythos/defines.hpp>
#include <mythos/math/vec4.hpp>

#include <vulkan/vulkan.h>

namespace myl::vulkan {
	class context; // fdw declaration

	enum class render_pass_state {
		ready,
		recording,
		in_render_pass,
		recording_ended,
		submitted,
		not_allocated
	};

	class render_pass {
		context& m_context; // context must outlive render_pass

		VkRenderPass m_handle;
		f32 m_x, m_y, m_w, m_h;
		f32vec4 m_color;

		f32 m_depth;
		u32 m_stencil;

		render_pass_state m_state;
	public:
		render_pass(context&, f32 a_x, f32 a_y, f32 a_w, f32 a_h, const f32vec4& a_color, f32 a_depth, u32 a_stencil);
		~render_pass();

		void begin(command_buffer&, VkFramebuffer);
		void end(command_buffer&);
	};
}
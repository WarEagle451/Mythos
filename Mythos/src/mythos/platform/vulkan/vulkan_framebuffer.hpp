#pragma once
#include "vulkan_utils.hpp"
#include "vulkan_render_pass.hpp"

#include <mythos/math/vec2.hpp>

#include <vector>

namespace myl::vulkan {
	class context; // fwd declaration

	class framebuffer {
		context& m_context; // Context must outlive framebuffer

		VkFramebuffer m_handle;
		std::vector<VkImageView> m_attachments;
		render_pass& m_render_pass; // render_pass must outlive framebuffer
	public:
		framebuffer(context&, render_pass&, const u32vec2& a_size, const std::vector<VkImageView>&);
		~framebuffer();

		framebuffer(const framebuffer&) = delete;
		framebuffer& operator=(const framebuffer&) = delete;

		VkFramebuffer& handle() { return m_handle; }
	};
}
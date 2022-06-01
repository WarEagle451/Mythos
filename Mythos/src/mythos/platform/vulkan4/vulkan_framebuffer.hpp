#pragma once
#include "vulkan_render_pass.hpp"

#include <vector>

namespace myl::vulkan4 {
	class context; // fwd declaration

	class framebuffer {
		context& m_context;

		VkFramebuffer m_handle = VK_NULL_HANDLE;
		std::vector<VkImageView> m_attachments;
		render_pass& m_render_pass;
	public:
		framebuffer(context&, render_pass&, u32 a_width, u32 a_height, const std::vector<VkImageView>& a_attachments);
		~framebuffer();

		VkFramebuffer& handle() { return m_handle; }
	};
}

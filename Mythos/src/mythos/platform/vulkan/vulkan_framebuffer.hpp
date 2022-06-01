#pragma once
#include "vulkan_render_pass.hpp"

#include <vector>

namespace myl::vulkan {
	class context; // fwd declaration

	class framebuffer {
		context& m_context;

		VkFramebuffer m_handle = VK_NULL_HANDLE;
		std::vector<VkImageView> m_attachments;
		render_pass& m_render_pass;
	public:
		framebuffer(context&, render_pass&, const VkExtent2D&, const std::vector<VkImageView>& a_attachments);
		~framebuffer();

		VkFramebuffer& handle() { return m_handle; }
	};
}

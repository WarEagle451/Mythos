#include "vulkan_framebuffer.hpp"
#include "vulkan_context.hpp"

namespace myl::vulkan {
	framebuffer::framebuffer(context& a_context, render_pass& a_render_pass, const u32vec2& a_size, const std::vector<VkImageView>& a_attachments)
		: m_context(a_context)
		, m_render_pass(a_render_pass)
		, m_attachments(a_attachments) {
		VkFramebufferCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = m_render_pass.handle(),
			.attachmentCount = static_cast<u32>(m_attachments.size()),
			.pAttachments = m_attachments.data(),
			.width = a_size.w,
			.height = a_size.h,
			.layers = 1
		};

		MYL_VK_CHECK(vkCreateFramebuffer, m_context.device().logical(), &info, nullptr, &m_handle);
	}

	framebuffer::~framebuffer() {
		vkDestroyFramebuffer(m_context.device().logical(), m_handle, nullptr);
	}
}
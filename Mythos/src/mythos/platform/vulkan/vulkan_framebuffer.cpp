#include "vulkan_framebuffer.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"

namespace myl::vulkan {
	framebuffer::framebuffer(context& a_context, render_pass& a_render_pass, const VkExtent2D& a_extent, const std::vector<VkImageView>& a_attachments)
		: m_context(a_context)
		, m_render_pass(a_render_pass)
		, m_attachments(a_attachments) {
		VkFramebufferCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = m_render_pass.handle(),
			.attachmentCount = static_cast<u32>(m_attachments.size()),
			.pAttachments = m_attachments.data(),
			.width = a_extent.width,
			.height = a_extent.height,
			.layers = 1
		};

		MYL_VK_ASSERT(vkCreateFramebuffer, m_context.device(), &info, VK_NULL_HANDLE, &m_handle);
	}

	framebuffer::~framebuffer() {
		vkDestroyFramebuffer(m_context.device(), m_handle, VK_NULL_HANDLE);
	}
}
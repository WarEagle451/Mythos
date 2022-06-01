#include "vulkan_framebuffer.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"

namespace myl::vulkan {
	framebuffer::framebuffer(context& a_context, render_pass& a_render_pass, u32 a_width, u32 a_height, const std::vector<VkImageView>& a_attachments)
		: m_context(a_context)
		, m_render_pass(a_render_pass)
		, m_attachments(a_attachments) {

		// Creation info
		VkFramebufferCreateInfo framebuffer_create_info{
			.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
			.renderPass = m_render_pass.m_handle,
			.attachmentCount = static_cast<u32>(m_attachments.size()),
			.pAttachments = m_attachments.data(),
			.width = a_width,
			.height = a_height,
			.layers = 1
		};

		MYL_VK_ASSERT(vkCreateFramebuffer, m_context.m_device, &framebuffer_create_info, VK_NULL_HANDLE, &m_handle);
	}

	framebuffer::~framebuffer() {
		if (m_handle != VK_NULL_HANDLE)
			vkDestroyFramebuffer(m_context.m_device, m_handle, VK_NULL_HANDLE);
	}
}
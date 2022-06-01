#include "vulkan_render_pass.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_swapchain.hpp"

#include <vector>

namespace myl::vulkan {
	render_pass::render_pass(swapchain& a_swapchain, context& a_context, f32 a_x, f32 a_y, f32 a_w, f32 a_h, const f32vec4& a_color, f32 a_depth, u32 a_stencil)
		: m_context(a_context)
		, m_x(a_x)
		, m_y(a_y)
		, m_w(a_w)
		, m_h(a_h)
		, m_color(a_color)
		, m_depth(a_depth)
		, m_stencil(a_stencil)
		, m_state(render_pass_state::not_allocated) {

		// Attachments
		/// MYTodo: Make render_pass attachments configurable
		const u32 attachment_description_count = 2;
		std::vector<VkAttachmentDescription> attachment_description{};
		attachment_description.reserve(attachment_description_count);

		// Color attachment
		attachment_description.push_back(VkAttachmentDescription{
				.flags = 0,
				.format = a_swapchain.image_format().format, /// MYTodo: Configurable
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // Do not expect any particular layout before render_pass starts
				.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // Transitioned to after render_pass
			});

		VkAttachmentReference color_attachment_ref{
			.attachment = 0, // Attachment array index
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		// Depth attachment
		attachment_description.push_back(VkAttachmentDescription{
				.flags = 0,
				.format = a_swapchain.depth_format(),
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
				.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
				.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
				.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
				.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			});

		VkAttachmentReference depth_attachment_ref{
			.attachment = 1, // Attachment array index
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		/// MYTodo: Other attachment types, input, resolve, preserve

		VkSubpassDescription subpass{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = 1,
			.pColorAttachments = &color_attachment_ref,
			.pResolveAttachments = nullptr,
			.pDepthStencilAttachment = &depth_attachment_ref,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr,
		};

		// render_pass dependencies
		/// MYTodo: configurable
		VkSubpassDependency dependency{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dependencyFlags = 0
		};

		VkRenderPassCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.attachmentCount = attachment_description_count,
			.pAttachments = attachment_description.data(),
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 1,
			.pDependencies = &dependency
		};

		MYL_VK_CHECK(vkCreateRenderPass, m_context.device().logical(), &create_info, nullptr, &m_handle);
		m_state = render_pass_state::ready;
	}

	render_pass::~render_pass() {
		if (m_handle)
			vkDestroyRenderPass(m_context.device().logical(), m_handle, nullptr);
	}

	void render_pass::begin(command_buffer& a_command_buffer, VkFramebuffer a_framebuffer) {
		VkClearValue clear_values[2]{};
		clear_values[0].color.float32[0] = m_color.r;
		clear_values[0].color.float32[1] = m_color.g;
		clear_values[0].color.float32[2] = m_color.b;
		clear_values[0].color.float32[3] = m_color.a;
		clear_values[1].depthStencil.depth = m_depth;
		clear_values[1].depthStencil.stencil = m_stencil;

		VkRenderPassBeginInfo info{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = m_handle,
			.framebuffer = a_framebuffer,
			.renderArea = {
				.offset = { static_cast<i32>(m_x), static_cast<i32>(m_y) },
				.extent = { static_cast<u32>(m_w), static_cast<u32>(m_h) }
			},
			.clearValueCount = 2,
			.pClearValues = clear_values
		};

		vkCmdBeginRenderPass(a_command_buffer.handle(), &info, VK_SUBPASS_CONTENTS_INLINE);
		a_command_buffer.set_state(command_buffer_state::in_render_pass);
	}

	void render_pass::end(command_buffer& a_command_buffer) {
		vkCmdEndRenderPass(a_command_buffer.handle());
		a_command_buffer.set_state(command_buffer_state::recording);
	}
}

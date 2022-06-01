#include "vulkan_render_pass.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_swapchain.hpp"

namespace myl::vulkan {
	render_pass::render_pass(context& a_context, swapchain& a_swapchain, const f32vec4& a_color, f32 a_x, f32 a_y, const VkExtent2D& a_extent, f32 a_depth, u32 a_stencil)
		: m_context(a_context)
		, m_x(a_x)
		, m_y(a_y)
		, m_extent(a_extent)
		, m_color(a_color)
		, m_depth(a_depth)
		, m_stencil(a_stencil)
		, m_state(render_pass_state::ready) {
		std::vector<VkAttachmentDescription> attachment_descriptions;
		attachment_descriptions.push_back(VkAttachmentDescription{ // Color attachment
			.flags = 0,
			.format = a_swapchain.image_format().format,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED, // Do not expect any particular layout before render pass starts.
			.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR // Transitioned to after the render pass
		});

		VkAttachmentReference color_attachment_reference{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};

		attachment_descriptions.push_back(VkAttachmentDescription{ // Depth attachment
			.format = m_context.depth_format(),
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
			.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		});
		
		VkAttachmentReference depth_attachment_reference{
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		/// MYTODO: other attachment types (input, resolve, preserve)
		VkSubpassDescription subpass{
			.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
			.inputAttachmentCount = 0,
			.pInputAttachments = nullptr,
			.colorAttachmentCount = 1,
			.pColorAttachments = &color_attachment_reference,
			.pResolveAttachments = nullptr,
			.pDepthStencilAttachment = &depth_attachment_reference,
			.preserveAttachmentCount = 0,
			.pPreserveAttachments = nullptr
		};

		/// Render pass dependencies. MYTODO: make this configurable.
		VkSubpassDependency dependency{
			.srcSubpass = VK_SUBPASS_EXTERNAL,
			.dstSubpass = 0,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = 0,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dependencyFlags = 0
		};

		VkRenderPassCreateInfo render_pass_create_info{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.flags = 0,
			.attachmentCount = static_cast<u32>(attachment_descriptions.size()),
			.pAttachments = attachment_descriptions.data(),
			.subpassCount = 1,
			.pSubpasses = &subpass,
			.dependencyCount = 1,
			.pDependencies = &dependency,
		};

		MYL_VK_ASSERT(vkCreateRenderPass, m_context.device(), &render_pass_create_info, VK_NULL_HANDLE, &m_handle);
	}

	render_pass::~render_pass() {
		if (m_handle)
			vkDestroyRenderPass(m_context.device(), m_handle, VK_NULL_HANDLE);
	}

	void render_pass::begin(command_buffer* a_command_buffer, VkFramebuffer a_framebuffer) {
		std::vector<VkClearValue> clear_values(2);
		clear_values[0].color.float32[0] = m_color.r;
		clear_values[0].color.float32[1] = m_color.g;
		clear_values[0].color.float32[2] = m_color.b;
		clear_values[0].color.float32[3] = m_color.a;

		clear_values[1].depthStencil.depth = m_depth;
		clear_values[1].depthStencil.stencil = m_stencil;

		VkRenderPassBeginInfo begin_info{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = m_handle,
			.framebuffer = a_framebuffer,
			.renderArea = {
				.offset = { static_cast<i32>(m_x), static_cast<i32>(m_y) },
				.extent = m_extent
			},
			.clearValueCount = static_cast<u32>(clear_values.size()),
			.pClearValues = clear_values.data()
		};

		vkCmdBeginRenderPass(a_command_buffer->handle(), &begin_info, VK_SUBPASS_CONTENTS_INLINE);
		a_command_buffer->set_state(command_buffer_state::in_render_pass);
	}

	void render_pass::end(command_buffer* a_command_buffer) {
		vkCmdEndRenderPass(a_command_buffer->handle());
		a_command_buffer->set_state(command_buffer_state::recording);
	}
}
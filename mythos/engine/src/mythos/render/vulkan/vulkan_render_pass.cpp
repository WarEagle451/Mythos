#include <mythos/render/vulkan/vulkan_render_pass.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace myth::vulkan {
    auto render_pass::create(render_pass* h, device& device, const create_info& ci, VkAllocationCallbacks* allocator) -> void {
        std::vector<VkAttachmentDescription> attachment_descriptions{
            VkAttachmentDescription { // Color attachment
                //.flags          = ,
                .format         = ci.color_format,
                .samples        = VK_SAMPLE_COUNT_1_BIT,
                .loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR,
                .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
                .stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
                .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
                .initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED,
                .finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
            }
        };

        VkAttachmentReference color_attachment_reference{
            .attachment = 0,
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };

        VkSubpassDescription subpass_description{
            //.flags = ,
            .pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS,
            //.inputAttachmentCount    = ,
            //.pInputAttachments       = ,
            .colorAttachmentCount    = 1,
            .pColorAttachments       = &color_attachment_reference,
            //.pResolveAttachments     = ,
            //.pDepthStencilAttachment = ,
            //.preserveAttachmentCount = ,
            //.pPreserveAttachments    = 
        };

        VkSubpassDependency subpass_dependency{
            .srcSubpass      = VK_SUBPASS_EXTERNAL,
            .dstSubpass      = 0,
            .srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask   = 0,
            .dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            //.dependencyFlags = 
        };

        VkRenderPassCreateInfo render_pass_create_info{
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            //.pNext           =,
            //.flags           =,
            .attachmentCount = static_cast<uint32_t>(attachment_descriptions.size()),
            .pAttachments    = attachment_descriptions.data(),
            .subpassCount    = 1,
            .pSubpasses      = &subpass_description,
            .dependencyCount = 1,
            .pDependencies   = &subpass_dependency
        };

        MYTHOS_VULKAN_VERIFY(vkCreateRenderPass, device.logical(), &render_pass_create_info, allocator, &h->m_render_pass);

        h->m_clear_color = ci.clear_color;
    }

    auto render_pass::destroy(render_pass* h, device& device, VkAllocationCallbacks* allocator) noexcept -> void {
        if (h->m_render_pass)
            vkDestroyRenderPass(device.logical(), h->m_render_pass, allocator);
    }

    auto render_pass::set_clear_color(const VkClearColorValue& color) -> void {
        m_clear_color = color;
    }

    auto render_pass::begin(VkCommandBuffer command_buffer, VkFramebuffer framebuffer, const VkRect2D& render_area) -> void {
        VkClearValue clear_values = {
            .color = m_clear_color
        };

        VkRenderPassBeginInfo render_pass_begin_info{
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
            //.pNext           = ,
            .renderPass      = m_render_pass,
            .framebuffer     = framebuffer,
            .renderArea      = render_area,
            .clearValueCount = 1,
            .pClearValues    = &clear_values,
        };

        vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
    }

    auto render_pass::end(VkCommandBuffer command_buffer) -> void {
        vkCmdEndRenderPass(command_buffer);
    }
}

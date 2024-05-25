#include <mythos/render/vulkan/vulkan_render_pass.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace myth::vulkan {
    MYL_NO_DISCARD render_pass::render_pass(context& context, VkFormat color_format)
        : m_context{ context } {
        std::vector<VkAttachmentDescription> attachment_descriptions{
            VkAttachmentDescription { // Color attachment
                //.flags          = ,
                .format         = color_format,
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

        VkRenderPassCreateInfo render_pass_create_info{
            .sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
            //.pNext           =,
            //.flags           =,
            .attachmentCount = static_cast<uint32_t>(attachment_descriptions.size()),
            .pAttachments    = attachment_descriptions.data(),
            .subpassCount    = 1,
            .pSubpasses      = &subpass_description,
            //.dependencyCount = ,
            //.pDependencies   = 
        };

        MYTHOS_VULKAN_VERIFY(vkCreateRenderPass, m_context.device(), &render_pass_create_info, VK_NULL_HANDLE, &m_render_pass);
    }

    render_pass::~render_pass() {
        vkDestroyRenderPass(m_context.device(), m_render_pass, VK_NULL_HANDLE);
    }
}

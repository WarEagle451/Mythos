#include <mythos/render/vulkan/vulkan_pipeline.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace myth::vulkan {
    MYL_NO_DISCARD pipeline::pipeline(
        context& context,
        VkRenderPass render_pass,
        const VkViewport& viewport,
        const VkRect2D& scissor,
        const VkPrimitiveTopology primitive,
        const std::vector<VkPipelineShaderStageCreateInfo>& shader_stage_create_infos)
        : m_context{ context } {
        std::vector<VkDynamicState> dynamic_states{
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamic_state_create_info{
            .sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            //.pNext = ,
            //.flags = ,
            .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
            .pDynamicStates    = dynamic_states.data()
        };

        VkPipelineViewportStateCreateInfo viewport_state_create_info{
            .sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            //.pNext =,
            //.flags =,
            .viewportCount = 1,
            .pViewports    = &viewport,
            .scissorCount  = 1,
            .pScissors     = &scissor
        };

        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{
            .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            //.pNext = ,
            //.flags = ,
            .vertexBindingDescriptionCount   = 0,
            .pVertexBindingDescriptions      = nullptr,
            .vertexAttributeDescriptionCount = 0,
            .pVertexAttributeDescriptions    = nullptr
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            //.pNext = ,
            //.flags = ,
            .topology               = primitive,
            .primitiveRestartEnable = VK_FALSE
        };

        VkPipelineRasterizationStateCreateInfo rasterization_state_create_info{
            .sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            //.pNext =,
            //.flags =,
            .depthClampEnable        = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode             = VK_POLYGON_MODE_FILL,
            .cullMode                = VK_CULL_MODE_BACK_BIT,
            .frontFace               = VK_FRONT_FACE_CLOCKWISE,
            .depthBiasEnable         = VK_FALSE,
            .depthBiasConstantFactor = 0.f,
            .depthBiasClamp          = 0.f,
            .depthBiasSlopeFactor    = 0.f,
            .lineWidth               = 1.f
        };

        VkPipelineMultisampleStateCreateInfo multisample_state_create_info{
            .sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            //.pNext =,
            //.flags =,
            .rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT,
            .sampleShadingEnable   = VK_FALSE,
            .minSampleShading      = 1.f,
            .pSampleMask           = nullptr,
            .alphaToCoverageEnable = VK_FALSE,
            .alphaToOneEnable      = VK_FALSE
        };

        VkPipelineColorBlendAttachmentState color_blend_attachment_state{
            .blendEnable         = VK_FALSE, /// MYTODO: Enable color blending, example https://vulkan-tutorial.com/en/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions
            .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
            .colorBlendOp        = VK_BLEND_OP_ADD,
            .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
            .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
            .alphaBlendOp        = VK_BLEND_OP_ADD,
            .colorWriteMask      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
        };

        VkPipelineColorBlendStateCreateInfo color_blend_state_create_info{
            .sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            //.pNext =,
            //.flags =,
            .logicOpEnable     = VK_FALSE,
            .logicOp           = VK_LOGIC_OP_COPY,
            .attachmentCount   = 1,
            .pAttachments      = &color_blend_attachment_state,
            .blendConstants    = { 0.f, 0.f, 0.f, 0.f }
        };

        VkPipelineLayoutCreateInfo  layout_create_info{
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            //.pNext = ,
            //.flags = ,
            .setLayoutCount         = 0,
            .pSetLayouts            = nullptr,
            .pushConstantRangeCount = 0,
            .pPushConstantRanges    = nullptr
        };

        MYTHOS_VULKAN_VERIFY(vkCreatePipelineLayout, m_context.device(), &layout_create_info, VK_NULL_HANDLE, &m_layout);

        VkGraphicsPipelineCreateInfo pipeline_create_info{
            .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            //.pNext =,
            //.flags =,
            .stageCount          = static_cast<uint32_t>(shader_stage_create_infos.size()),
            .pStages             = shader_stage_create_infos.data(),
            .pVertexInputState   = &vertex_input_state_create_info,
            .pInputAssemblyState = &input_assembly_state_create_info,
            //.pTessellationState  = ,
            .pViewportState      = &viewport_state_create_info,
            .pRasterizationState = &rasterization_state_create_info,
            .pMultisampleState   = &multisample_state_create_info,
            .pDepthStencilState  = nullptr,
            .pColorBlendState    = &color_blend_state_create_info,
            .pDynamicState       = &dynamic_state_create_info,
            .layout              = m_layout,
            .renderPass          = render_pass,
            .subpass             = 0,
            .basePipelineHandle  = VK_NULL_HANDLE,
            .basePipelineIndex   = -1
        };

        MYTHOS_VULKAN_VERIFY(vkCreateGraphicsPipelines, m_context.device(), VK_NULL_HANDLE, 1, &pipeline_create_info, VK_NULL_HANDLE, &m_pipeline);
    }

    pipeline::~pipeline() {
        if (m_pipeline)
            vkDestroyPipeline(m_context.device(), m_pipeline, VK_NULL_HANDLE);
        if (m_layout)
            vkDestroyPipelineLayout(m_context.device(), m_layout, VK_NULL_HANDLE);
    }

    auto pipeline::bind(VkCommandBuffer command_buffer) -> void {
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    }
}

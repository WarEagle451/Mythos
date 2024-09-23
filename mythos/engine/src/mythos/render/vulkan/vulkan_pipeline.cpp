#include <mythos/log.hpp>
#include <mythos/render/vulkan/vulkan_pipeline.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

namespace myth::vulkan {
    MYL_NO_DISCARD static constexpr auto shader_data_type_to_VkFormat(shader_data_type type) -> VkFormat {
        switch (type) {
            using enum shader_data_type;
            case none:
                MYTHOS_FATAL("shader data type has no type!");
                return VK_FORMAT_UNDEFINED;
            case boolean: return VK_FORMAT_R32_SINT;
            case int1:    return VK_FORMAT_R32_SINT;
            case int2:    return VK_FORMAT_R32G32_SINT;
            case int3:    return VK_FORMAT_R32G32B32_SINT;
            case int4:    return VK_FORMAT_R32G32B32A32_SINT;
            case float1:  return VK_FORMAT_R32_SFLOAT;
            case float2:  return VK_FORMAT_R32G32_SFLOAT;
            case float3:  return VK_FORMAT_R32G32B32_SFLOAT;
            case float4:  return VK_FORMAT_R32G32B32A32_SFLOAT;
            case mat3:
                MYTHOS_FATAL("Vulkan can't bind martices (3x3). Pass as individual columns");
                return VK_FORMAT_UNDEFINED;
            case mat4:
                MYTHOS_FATAL("Vulkan can't bind martices (4x4). Pass as individual columns");
                return VK_FORMAT_UNDEFINED;
            default:
                MYTHOS_FATAL("Unknown shader data type!");
                return VK_FORMAT_UNDEFINED;
        }
    }

    MYL_NO_DISCARD static constexpr auto shader_layout_to_VkVertexInputBindingDescriptions(const shader_layout& layout) -> std::vector<VkVertexInputBindingDescription> {
        return std::vector<VkVertexInputBindingDescription>{
            VkVertexInputBindingDescription{ /// MYTODO: Instanced rendering
                .binding   = 0,
                .stride    = layout.stride(),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
            }
        };
    }

    MYL_NO_DISCARD static constexpr auto shader_layout_to_VkVertexInputAttributeDescriptions(const shader_layout& layout) -> std::vector<VkVertexInputAttributeDescription> {
        std::vector<VkVertexInputAttributeDescription> out{};
        out.reserve(layout.elements().size());

        for (myl::u32 i = 0; i != layout.elements().size(); ++i) {
            const auto& e = layout.elements()[i];
            out.emplace_back(VkVertexInputAttributeDescription{
                .location = i,
                .binding  = 0,
                .format   = shader_data_type_to_VkFormat(e.type),
                .offset   = e.offset
            });
        }

        return out;
    }

    auto pipeline::create(pipeline* h, device& device, const create_info& ci, VkAllocationCallbacks* allocator) -> void {
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
            .pViewports    = &ci.viewport,
            .scissorCount  = 1,
            .pScissors     = &ci.scissor
        };

        auto bindings = shader_layout_to_VkVertexInputBindingDescriptions(ci.layout);
        auto attributes = shader_layout_to_VkVertexInputAttributeDescriptions(ci.layout);

        VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info{
            .sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            //.pNext = ,
            //.flags = ,
            .vertexBindingDescriptionCount   = static_cast<uint32_t>(bindings.size()),
            .pVertexBindingDescriptions      = bindings.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size()),
            .pVertexAttributeDescriptions    = attributes.data()
        };

        VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info{
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            //.pNext = ,
            //.flags = ,
            .topology               = ci.primitive,
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
            .setLayoutCount         = static_cast<uint32_t>(ci.descriptor_set_layouts.size()),
            .pSetLayouts            = ci.descriptor_set_layouts.data(),
            .pushConstantRangeCount = 0,
            .pPushConstantRanges    = nullptr
        };

        MYTHOS_VULKAN_VERIFY(vkCreatePipelineLayout, device.logical(), &layout_create_info, allocator, &h->m_layout);

        VkGraphicsPipelineCreateInfo pipeline_create_info{
            .sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            //.pNext =,
            //.flags =,
            .stageCount          = static_cast<uint32_t>(ci.shader_stage_create_infos.size()),
            .pStages             = ci.shader_stage_create_infos.data(),
            .pVertexInputState   = &vertex_input_state_create_info,
            .pInputAssemblyState = &input_assembly_state_create_info,
            //.pTessellationState  = ,
            .pViewportState      = &viewport_state_create_info,
            .pRasterizationState = &rasterization_state_create_info,
            .pMultisampleState   = &multisample_state_create_info,
            .pDepthStencilState  = nullptr,
            .pColorBlendState    = &color_blend_state_create_info,
            .pDynamicState       = &dynamic_state_create_info,
            .layout              = h->m_layout,
            .renderPass          = ci.render_pass,
            .subpass             = 0,
            .basePipelineHandle  = VK_NULL_HANDLE,
            .basePipelineIndex   = -1
        };

        MYTHOS_VULKAN_VERIFY(vkCreateGraphicsPipelines, device.logical(), VK_NULL_HANDLE, 1, &pipeline_create_info, allocator, &h->m_pipeline);
    }

    auto pipeline::destroy(pipeline* h, device& device, VkAllocationCallbacks* allocator) noexcept -> void {
        if (h->m_pipeline)
            vkDestroyPipeline(device.logical(), h->m_pipeline, allocator);
        if (h->m_layout)
            vkDestroyPipelineLayout(device.logical(), h->m_layout, allocator);
    }

    auto pipeline::bind(VkCommandBuffer command_buffer, const VkDescriptorSet& descriptor_set) -> void {
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_layout, 0, 1, &descriptor_set, 0, nullptr);
    }

    auto pipeline::bind(VkCommandBuffer command_buffer, const std::vector<VkDescriptorSet>& descriptor_sets) -> void {
        vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
        vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_layout, 0, static_cast<uint32_t>(descriptor_sets.size()), descriptor_sets.data(), 0, nullptr);
    }
}

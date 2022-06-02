#include "vulkan_pipeline.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_vertex_array.hpp"

#include <mythos/math/vec3.hpp>

namespace myl::vulkan {
	pipeline::pipeline(context& a_context, render_pass& a_render_pass, const std::vector<VkVertexInputAttributeDescription>& a_attributes, const std::vector<VkDescriptorSetLayout>& a_descriptor_layouts, const std::vector<VkPipelineShaderStageCreateInfo>& a_stages, VkViewport& a_viewport, VkRect2D& a_scissor, bool a_is_wireframe)
		: m_context(a_context) {
		VkPipelineViewportStateCreateInfo viewport_state{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = &a_viewport,
			.scissorCount = 1,
			.pScissors = &a_scissor,
		};

		VkPipelineRasterizationStateCreateInfo rasterizer_create_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = a_is_wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.f,
			.depthBiasClamp = 0.f,
			.depthBiasSlopeFactor = 0.f,
			.lineWidth = 1.f
		};

		VkPipelineMultisampleStateCreateInfo multisampling_create_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.f,
			.pSampleMask = VK_NULL_HANDLE,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};

		VkPipelineDepthStencilStateCreateInfo depth_stencil{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE
		};

		VkPipelineColorBlendAttachmentState color_blend_attachment_state{
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};

		VkPipelineColorBlendStateCreateInfo color_blend_state_create_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &color_blend_attachment_state
		};

		std::vector<VkDynamicState> dynamic_states{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		VkPipelineDynamicStateCreateInfo dynamic_state_create_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = static_cast<u32>(dynamic_states.size()),
			.pDynamicStates = dynamic_states.data()
		};

		auto binding_descriptions = vertex::get_binding_descriptions();

		VkPipelineVertexInputStateCreateInfo vertex_input_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = static_cast<u32>(binding_descriptions.size()),
			.pVertexBindingDescriptions = binding_descriptions.data(),
			.vertexAttributeDescriptionCount = static_cast<u32>(a_attributes.size()),
			.pVertexAttributeDescriptions = a_attributes.data(),
		};

		VkPipelineInputAssemblyStateCreateInfo input_assembly{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		VkPipelineLayoutCreateInfo pipeline_layout_create_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<u32>(a_descriptor_layouts.size()),
			.pSetLayouts = a_descriptor_layouts.data()
		};

		MYL_VK_ASSERT(vkCreatePipelineLayout, m_context.device(), &pipeline_layout_create_info, VK_NULL_HANDLE, &m_pipeline_layout);

		VkGraphicsPipelineCreateInfo pipeline_create_info{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.stageCount = static_cast<u32>(a_stages.size()),
			.pStages = a_stages.data(),
			.pVertexInputState = &vertex_input_info,
			.pInputAssemblyState = &input_assembly,
			.pTessellationState = VK_NULL_HANDLE,
			.pViewportState = &viewport_state,
			.pRasterizationState = &rasterizer_create_info,
			.pMultisampleState = &multisampling_create_info,
			.pDepthStencilState = &depth_stencil,
			.pColorBlendState = &color_blend_state_create_info,
			.pDynamicState = &dynamic_state_create_info,
			.layout = m_pipeline_layout,
			.renderPass = a_render_pass.handle(),
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1,
		};

		MYL_VK_VERIFY(vkCreateGraphicsPipelines, m_context.device(), VK_NULL_HANDLE, 1, &pipeline_create_info, VK_NULL_HANDLE, &m_handle);
	}

	pipeline::~pipeline() {
		if (m_pipeline_layout)
			vkDestroyPipelineLayout(m_context.device(), m_pipeline_layout, VK_NULL_HANDLE);
		if (m_handle)
			vkDestroyPipeline(m_context.device(), m_handle, VK_NULL_HANDLE);
	}

	void pipeline::bind(command_buffer& a_command_buffer, VkPipelineBindPoint a_bind_point) {
		vkCmdBindPipeline(a_command_buffer.handle(), a_bind_point, m_handle);
	}
}
#include "vulkan_pipeline.hpp"
#include "vulkan_context.hpp"
#include <mythos/platform/vulkan/vulkan_utils.hpp>
#include <mythos/platform/vulkan/vulkan_vertex_array.hpp>

namespace myl::vulkane {
	pipeline::pipeline(context& a_context, render_pass& a_render_pass,
		const std::vector<VkVertexInputAttributeDescription>& a_attributes,
		const std::vector<VkDescriptorSetLayout>& a_descriptor_set_layouts,
		const std::vector<VkPipelineShaderStageCreateInfo>& a_stages,
		const VkViewport& a_viewport,
		const VkRect2D& a_scissor,
		bool a_is_wireframe)
		: m_context(a_context) {

		VkPipelineViewportStateCreateInfo viewport_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = &a_viewport,
			.scissorCount = 1,
			.pScissors = &a_scissor
		};

		VkPipelineRasterizationStateCreateInfo rasterization_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE,
			.rasterizerDiscardEnable = VK_FALSE,
			.polygonMode = a_is_wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE, /// MYTodo: If geometry is rendering backwards or inside out, this is why
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.f,
			.depthBiasClamp = 0.f,
			.depthBiasSlopeFactor = 0.f,
			.lineWidth = 1.f
		};
		
		VkPipelineMultisampleStateCreateInfo multisample_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			.minSampleShading = 1.f,
			.pSampleMask = VK_NULL_HANDLE,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};

		VkPipelineDepthStencilStateCreateInfo depth_stencil_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_TRUE,
			.depthWriteEnable = VK_TRUE,
			.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE
		};

		VkPipelineColorBlendAttachmentState color_blend_attachment{
			.blendEnable = VK_TRUE,
			.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.colorBlendOp = VK_BLEND_OP_ADD,
			.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
			.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
			.alphaBlendOp = VK_BLEND_OP_ADD,
			.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};
		VkPipelineColorBlendStateCreateInfo color_blend_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
			.logicOpEnable = VK_FALSE,
			.logicOp = VK_LOGIC_OP_COPY,
			.attachmentCount = 1,
			.pAttachments = &color_blend_attachment,
			.blendConstants = { 0.f, 0.f, 0.f, 0.f }
		};

		std::vector<VkDynamicState> dynamic_states{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};
		VkPipelineDynamicStateCreateInfo dynamic_state_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = static_cast<u32>(dynamic_states.size()),
			.pDynamicStates = dynamic_states.data()
		};

		auto binding_descriptions = vulkan::vertex::get_binding_descriptions();

		VkPipelineVertexInputStateCreateInfo vertex_input_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = static_cast<u32>(binding_descriptions.size()),
			.pVertexBindingDescriptions = binding_descriptions.data(),
			.vertexAttributeDescriptionCount = static_cast<u32>(a_attributes.size()),
			.pVertexAttributeDescriptions = a_attributes.data()
		};

		VkPipelineInputAssemblyStateCreateInfo input_assembly_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};

		VkPipelineLayoutCreateInfo layout_info{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<u32>(a_descriptor_set_layouts.size()),
			.pSetLayouts = a_descriptor_set_layouts.data(),
		};

		MYL_VK_ASSERT(vkCreatePipelineLayout, m_context.device().logical(), &layout_info, VK_NULL_HANDLE, &m_layout);

		VkGraphicsPipelineCreateInfo pipeline_info{
			.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
			.pNext = VK_NULL_HANDLE,
			.stageCount = static_cast<u32>(a_stages.size()),
			.pStages = a_stages.data(),
			.pVertexInputState = &vertex_input_info,
			.pInputAssemblyState = &input_assembly_info,
			.pTessellationState = VK_NULL_HANDLE,
			.pViewportState = &viewport_info,
			.pRasterizationState = &rasterization_info,
			.pMultisampleState = &multisample_info,
			.pDepthStencilState = &depth_stencil_info,
			.pColorBlendState = &color_blend_info,
			.pDynamicState = &dynamic_state_info,
			.layout = m_layout,
			.renderPass = a_render_pass.handle(),
			.subpass = 0,
			.basePipelineHandle = VK_NULL_HANDLE,
			.basePipelineIndex = -1
		};

		VkResult result = vkCreateGraphicsPipelines(m_context.device().logical(), VK_NULL_HANDLE, 1, & pipeline_info, VK_NULL_HANDLE, & m_handle);
		if (!vulkan::result_is_success(result))
			MYL_CORE_ERROR("Vulkan graphics pipline line creation failed. Result: {}", vulkan::VkResult_to_string(result));
	}

	pipeline::~pipeline() {
		if (m_layout != VK_NULL_HANDLE)
			vkDestroyPipelineLayout(m_context.device().logical(), m_layout, VK_NULL_HANDLE);
		if (m_handle != VK_NULL_HANDLE)
			vkDestroyPipeline(m_context.device().logical(), m_handle, VK_NULL_HANDLE);
	}

	void pipeline::bind(command_buffer& a_command_buffer, VkPipelineBindPoint a_bind_point) {
		vkCmdBindPipeline(a_command_buffer.handle(), a_bind_point, m_handle);
	}
}
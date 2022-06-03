#pragma once
#include "vulkan_command_buffer.hpp"
#include "vulkan_render_pass.hpp"

#include <vector>

namespace myl::vulkan {
	class context; // fwd declaration

	class pipeline {
		context& m_context;

		VkPipeline m_handle;
		VkPipelineLayout m_pipeline_layout;
	public:
		pipeline(context&, render_pass&, const std::vector<VkVertexInputAttributeDescription>&, const std::vector<VkDescriptorSetLayout>&, const std::vector<VkPipelineShaderStageCreateInfo>&, VkViewport&, VkRect2D& a_scissor, bool a_is_wireframe);
		~pipeline();

		VkPipelineLayout& layout() { return m_pipeline_layout; }

		void bind(command_buffer&, VkPipelineBindPoint);
	};
}
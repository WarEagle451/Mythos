#pragma once
#include "vulkan_command_buffer.hpp"
#include "vulkan_render_pass.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace myl::vulkan4 {
	class context; // fwd declaration

	class pipeline {
		context& m_context;

		VkPipeline m_handle;
		VkPipelineLayout m_pipeline_layout;
	public:
		pipeline(context&, render_pass&, const std::vector<VkVertexInputAttributeDescription>&, const std::vector<VkDescriptorSetLayout>&, const std::vector<VkPipelineShaderStageCreateInfo>&, VkViewport&, VkRect2D& a_scissor, bool a_is_wireframe);
		~pipeline();

		void bind(command_buffer&, VkPipelineBindPoint);
	};
}
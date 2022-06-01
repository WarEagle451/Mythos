#pragma once
#include "vulkan_render_pass.hpp"
#include "vulkan_command_buffer.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace myl::vulkan {
	class context; // fwd declaration

	class pipeline {
		context& m_context;

		VkPipeline m_handle{};
		VkPipelineLayout m_layout{};
	public: /// MYTodo: make a config out of that
		pipeline(context&, render_pass&,
			const std::vector<VkVertexInputAttributeDescription>&,
			const std::vector<VkDescriptorSetLayout>&,
			const std::vector<VkPipelineShaderStageCreateInfo>&,
			const VkViewport&,
			const VkRect2D& a_scissor,
			bool a_is_wireframe);
		~pipeline();

		void bind(command_buffer&, VkPipelineBindPoint);

	};
}
#pragma once
#include "vulkan_pipeline.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_render_pass.hpp"
#include "vulkan_buffer.hpp"

#include <mythos/render/shader.hpp>
#include <mythos/render/global_uniform_object.hpp>

#include <unordered_map>
#include <memory>

namespace myl::vulkan {
	class context; // fwd declaration

	class shader : public render::shader {
		context& m_context;

		std::unordered_map<render::shader_type, VkShaderModule> m_modules; /// MYTodo: These are not needed after pipeline is created, find a way to get rid of this member
		std::unique_ptr<pipeline> m_pipeline;

		VkDescriptorPool m_global_descriptor_pool;
		VkDescriptorSetLayout m_global_descriptor_set_layout;
		VkDescriptorSet m_global_descriptor_sets[3]; // One descriptor set per frame, max 3 for triple buffering

		std::unique_ptr<buffer> m_global_uniform_buffer;
		render::global_unifrom_object m_global_ubo;
	public:
		shader(context&, const std::filesystem::path&, swapchain&, render_pass&);
		~shader();

		render::global_unifrom_object& global_ubo() { return m_global_ubo; }

		void bind() const override;
		void unbind() const override;

		void update_global_state();
		void update(f32mat4x4); /// MYTemp:
	private:
		void create_module(VkShaderModuleCreateInfo&, VkShaderModule*);
	};
}
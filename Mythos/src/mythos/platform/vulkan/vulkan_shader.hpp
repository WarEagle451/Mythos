#pragma once
#include "vulkan_pipeline.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_render_pass.hpp"

#include <mythos/render/shader.hpp>

#include <unordered_map>
#include <memory>

namespace myl::vulkan {
	class context; // fwd declaration

	class shader : public render::shader {
		context& m_context;

		std::unordered_map<render::shader_type, VkShaderModule> m_modules;
		std::unique_ptr<pipeline> m_pipeline;
	public:
		shader(context&, const std::filesystem::path&, swapchain&, render_pass&);
		~shader();

		void bind() const override;
		void unbind() const override;
	private:
		void create_module(VkShaderModuleCreateInfo&, VkShaderModule*);
	};
}
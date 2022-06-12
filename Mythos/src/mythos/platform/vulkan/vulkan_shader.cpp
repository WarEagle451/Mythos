#include "vulkan_shader.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_vertex_array.hpp"

#include <mythos/io.hpp>
#include <mythos/core/log.hpp>

#include <shaderc/shaderc.hpp> /// MYTodo: switch to c api

namespace myl::vulkan {
	static const std::filesystem::path cache_directory() { /// MYTemp:
		return "cache/shaders";
	}

	static constexpr render::shader_type detect_shader_type(std::string_view type) {
		using enum render::shader_type;
		if (type == "vertex")
			return vertex;
		else if (type == "fragment" || type == "pixel")
			return fragment;
		else if (type == "compute")
			return compute;
		else if (type == "geometry")
			return geometry;
		else if (type == "tess_control")
			return tess_control;
		else if (type == "tess_evaluation")
			return tess_evaluation;
		else return unknown;
	}

	static constexpr shaderc_shader_kind shader_type_to_shaderc_shader_kind(render::shader_type type) {
		switch (type) {
			using enum render::shader_type;
			case vertex: return shaderc_vertex_shader;
			case fragment: return shaderc_fragment_shader;
			case compute: return shaderc_compute_shader;
			case geometry: return shaderc_geometry_shader;
			case tess_control: return shaderc_tess_control_shader;
			case tess_evaluation: return shaderc_tess_evaluation_shader;
			case unknown: return shaderc_glsl_infer_from_source;
			default: return shaderc_glsl_infer_from_source;
		}
	}

	static std::unordered_map<render::shader_type, std::string> process(std::string_view src) {
		constexpr const char* token = "#type";
		constexpr size_t token_length = 5;

		std::unordered_map<render::shader_type, std::string> sources{};
		std::size_t pos = src.find(token, 0); // start of first shader type declaration line
		while (pos != std::string::npos) {
			size_t eol = src.find("\r\n", pos); // end of shader type declaration line
			MYL_CORE_ASSERT(eol != std::string::npos, "Syntax error");

			size_t begin = pos + token_length + 1; // start of shader type name (after "#type " keyword)
			render::shader_type type = detect_shader_type(src.substr(begin, eol - begin)); // getting type string and detecting the type
			MYL_CORE_ASSERT(type != render::shader_type::unknown, "Invalid shader type specified");

			size_t next_line_pos = src.find_first_not_of("\r\n", eol); // beginning of shader code after type declaration
			MYL_CORE_ASSERT(next_line_pos != std::string::npos, "Syntax error");
			pos = src.find(token, next_line_pos); // beginning of next shader type declaration line

			sources[type] = (pos == std::string::npos) ?
				src.substr(next_line_pos) : // beginning of shader to end of file
				src.substr(next_line_pos, pos - next_line_pos); // beginning od shader code to line before next shader type declaration line
		}

		return sources;
	}

	static std::unordered_map<render::shader_type, std::vector<u32>> get_or_compile_binaries(const std::filesystem::path& file) {
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_3);

		bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_size);

		std::unordered_map<render::shader_type, std::vector<u32>> binaries;
		auto sources = process(load_into_memory(file));
		for (auto&& [type, src] : sources) {
			std::filesystem::path cached = cache_directory() / (file.stem().string() + ".vulk" + render::shader_type_to_extenstion(type) + ".spv");

			if (std::vector<u32> bin = load_into_memory_binary<u32>(cached); bin.empty()) { // Compiling shader code
				auto result = compiler.CompileGlslToSpv(src, shader_type_to_shaderc_shader_kind(type), file.string().c_str(), options);
				MYL_CORE_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success, "Failed to compile Vulkan shader");
				binaries[type] = std::vector<u32>(result.begin(), result.end());

				std::filesystem::create_directories(cache_directory()); // Ensure the caching directory exists
				if (std::ofstream out(cached, std::ios::binary); out.is_open()) // Caching binary
					out.write(reinterpret_cast<char*>(binaries[type].data()), binaries[type].size() * sizeof(u32));
			}
			else binaries[type] = std::move(bin);
		}

		return binaries;
	}

	static constexpr VkShaderStageFlagBits shader_type_to_vulkan_shader_stage_bit(render::shader_type type) {
		switch (type) {
			using enum render::shader_type;
			case vertex: return VK_SHADER_STAGE_VERTEX_BIT;
			case fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case compute: return VK_SHADER_STAGE_COMPUTE_BIT;
			case geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
			case tess_control: return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case tess_evaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			case unknown: return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM; /// MYTemp: Is there an error bit?
			default: return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM; /// MYTemp: Is there an error bit?
		}
	}

	shader::shader(context& a_context, const std::filesystem::path& a_file, swapchain& a_swapchain, render_pass& a_render_pass)
		: render::shader()
		, m_context(a_context) {
		auto binaries = get_or_compile_binaries(a_file);
		for (auto&& [type, src] : binaries) { // Creating shader modules
			VkShaderModuleCreateInfo create_info{
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = src.size() * sizeof(u32),
				.pCode = src.data()
			};

			create_module(create_info, &m_modules[type]);
		}

		// Global descriptors
		VkDescriptorSetLayoutBinding global_ubo_layout_binding{
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT, /// MYTodo: This Means that is can only be used in the vertex stage, NOT GOOD! should be usable for all
			.pImmutableSamplers = VK_NULL_HANDLE
		};

		VkDescriptorSetLayoutCreateInfo global_layout_info{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = 1,
			.pBindings = &global_ubo_layout_binding
		};
		MYL_VK_ASSERT(vkCreateDescriptorSetLayout, m_context.device(), &global_layout_info, VK_NULL_HANDLE, &m_global_descriptor_set_layout);

		VkDescriptorPoolSize global_pool_size{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = static_cast<u32>(a_swapchain.images().size()), // could be hardcoded to 3
		};

		VkDescriptorPoolCreateInfo global_pool_info{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
			.maxSets = static_cast<u32>(a_swapchain.images().size()),
			.poolSizeCount = 1,
			.pPoolSizes = &global_pool_size,
		};
		MYL_VK_ASSERT(vkCreateDescriptorPool, m_context.device(), &global_pool_info, VK_NULL_HANDLE, &m_global_descriptor_pool);

		// Pipeline creation
		VkExtent2D swapchain_extent = m_context.framebuffer_extent();

		VkViewport viewport{
			.x = 0.f,
			.y = static_cast<f32>(swapchain_extent.height),
			.width = static_cast<f32>(swapchain_extent.width),
			.height = -static_cast<f32>(swapchain_extent.height),
			.minDepth = 0.f,
			.maxDepth = 1.f
		};

		VkRect2D scissor{
			.offset = {.x = 0, .y = 0 },
			.extent = swapchain_extent
		};

		// Attributes
		auto attribute_descriptions = vertex::get_attribute_descriptions();

		// Descriptors set layouts
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts{
			m_global_descriptor_set_layout
		};

		// Stages
		std::vector<VkPipelineShaderStageCreateInfo> shader_stages; /// MYTodo: Stored in shader?
		shader_stages.reserve(m_modules.size());
		for (auto&& [type, module] : m_modules)
			shader_stages.emplace_back(VkPipelineShaderStageCreateInfo{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
				.stage = shader_type_to_vulkan_shader_stage_bit(type),
				.module = module,
				.pName = "main"
				});

		m_pipeline = std::make_unique<pipeline>(m_context, a_render_pass, attribute_descriptions, descriptor_set_layouts, shader_stages, viewport, scissor, false);

		m_global_uniform_buffer = std::make_unique<buffer>(
			m_context,
			sizeof(render::global_unifrom_object) * 3,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			true);

		// Allocate global descriptor sets
		VkDescriptorSetLayout global_layouts[3]{
			m_global_descriptor_set_layout,
			m_global_descriptor_set_layout,
			m_global_descriptor_set_layout,
		};

		VkDescriptorSetAllocateInfo alloc_info{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
			.descriptorPool = m_global_descriptor_pool,
			.descriptorSetCount = 3,
			.pSetLayouts = global_layouts
		};
		MYL_VK_ASSERT(vkAllocateDescriptorSets, m_context.device(), &alloc_info, m_global_descriptor_sets);
	}

	shader::~shader() {
		m_global_uniform_buffer.reset();

		m_pipeline.reset();

		vkDestroyDescriptorPool(m_context.device(), m_global_descriptor_pool, VK_NULL_HANDLE);
		vkDestroyDescriptorSetLayout(m_context.device(), m_global_descriptor_set_layout, VK_NULL_HANDLE);

		for (auto&& [type, module] : m_modules)
			if (module != VK_NULL_HANDLE)
				vkDestroyShaderModule(m_context.device(), module, VK_NULL_HANDLE);
	}

	void shader::bind() const {
		m_pipeline->bind(m_context.graphics_command_buffers()[m_context.image_index()], VK_PIPELINE_BIND_POINT_GRAPHICS);
	}

	void shader::unbind() const {
		///MYTodo: Figure out how to unbind a pipeline
	}

	void shader::update_global_state() {
		u32 image_index = m_context.image_index();
		VkCommandBuffer cmd_buf = m_context.graphics_command_buffers()[image_index].handle();
		VkDescriptorSet global_descriptor = m_global_descriptor_sets[image_index];

		// Configure the descriptors for the given index
		u32 range = sizeof(render::global_unifrom_object);
		u64 offset = sizeof(render::global_unifrom_object) * image_index;

		m_global_uniform_buffer->load(offset, range, 0, &m_global_ubo);

		VkDescriptorBufferInfo buffer_info{
			.buffer = m_global_uniform_buffer->handle(),
			.offset = offset,
			.range = range
		};

		// Update descriptor sets
		VkWriteDescriptorSet descriptor_write{
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = m_global_descriptor_sets[image_index],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &buffer_info
		};

		vkUpdateDescriptorSets(m_context.device(), 1, &descriptor_write, 0, VK_NULL_HANDLE);
		// Bind the global descriptor set to be updated
		vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline->layout(), 0, 1, &global_descriptor, 0, VK_NULL_HANDLE);
	}

	void shader::update(f32mat4x4 a_model) {
		u32 image_index = m_context.image_index();
		VkCommandBuffer cmd_buf = m_context.graphics_command_buffers()[image_index].handle();

		vkCmdPushConstants(cmd_buf, m_pipeline->layout(), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(f32mat4x4), &a_model); // Spec only guarantess 128 bytes
	}

	void shader::create_module(VkShaderModuleCreateInfo& a_create_info, VkShaderModule* a_module) {
		if (vkCreateShaderModule(m_context.device(), &a_create_info, nullptr, a_module) != VK_SUCCESS)
			MYL_CORE_ERROR("Failed to create Vulkan shader module");
	}
}
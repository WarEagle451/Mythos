#include "vulkan_shader.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_vertex_array.hpp"

#include <mythos/file.hpp>
#include <mythos/core/log.hpp>

#include <shaderc/shaderc.hpp> /// MYTodo: switch to c api

#include <vector>
#include <fstream>

namespace myl::vulkan {
	static const std::filesystem::path cached_directory() { /// MYTemp:
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
			std::filesystem::create_directories(cached_directory()); // Ensures the cached directories exists
			std::filesystem::path cached = cached_directory() / (file.stem().string() + ".vulk" + render::shader_type_to_extenstion(type) + ".spv");

			if (std::ifstream in(cached, std::ios::in | std::ios::binary); in.is_open()) {
				in.seekg(0, std::ios::end);
				auto size = in.tellg();
				in.seekg(0, std::ios::beg);

				binaries[type] = std::vector<u32>{};
				binaries[type].resize(size / sizeof(u32));
				in.read(reinterpret_cast<char*>(binaries[type].data()), size);
			}
			else {
				auto result = compiler.CompileGlslToSpv(src, shader_type_to_shaderc_shader_kind(type), file.string().c_str(), options);
				MYL_CORE_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success, "Failed to compile Vulkan shader");

				binaries[type] = std::vector<u32>(result.begin(), result.end());

				if (std::ofstream out(cached, std::ios::binary); out.is_open()) // caching binary for later
					out.write(reinterpret_cast<char*>(binaries[type].data()), binaries[type].size() * sizeof(u32));
			}
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
			case unknown: return VK_SHADER_STAGE_MISS_BIT_KHR; /// MYTodo: I think these mean it's an error?
			default: return VK_SHADER_STAGE_MISS_BIT_KHR; /// MYTodo: I think these mean it's an error?
		}
	}

	shader::shader(context& a_context, const std::filesystem::path& a_file, swapchain& a_swapchain)
		: render::shader()
		, m_context(a_context) {
		/// MYTodo: Should check if file path is valid

		auto binaries = get_or_compile_binaries(a_file);
		for (auto&& [type, src] : binaries) { // Creating shader modules
			VkShaderModuleCreateInfo create_info{
				.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
				.codeSize = src.size() * sizeof(u32),
				.pCode = src.data()
			};

			create_module(create_info, &m_modules[type]);
		}

		/// MYTodo: Descriptors

		// Pipeline creation
		VkExtent2D swapchain_extent{
			.width = m_context.m_framebuffer_width,
			.height = m_context.m_framebuffer_height
		};

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

		/// MYTodo: Descriptors set layout

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

		/// MYTodo: a_swapchain.render_pass() is in context as main_render_pass for KOHI
		m_pipeline = std::make_unique<pipeline>(m_context, *m_context.main_renderpass, attribute_descriptions, std::vector<VkDescriptorSetLayout>(), shader_stages, viewport, scissor, false);
	}

	shader::~shader() {
		for (auto&& [type, module] : m_modules)
			if (module != VK_NULL_HANDLE)
				vkDestroyShaderModule(m_context.m_device, module, nullptr);
	}

	void shader::bind() const {
		m_pipeline->bind(m_context.graphics_command_buffers[m_context.image_index], VK_PIPELINE_BIND_POINT_GRAPHICS);
	}

	void shader::unbind() const {
		///MYTodo: Figure out how to unbind a pipeline
	}

	void shader::create_module(VkShaderModuleCreateInfo& a_create_info, VkShaderModule* a_module) {
		if (vkCreateShaderModule(m_context.m_device, &a_create_info, nullptr, a_module) != VK_SUCCESS)
			MYL_CORE_ERROR("Failed to create Vulkan shader module");
	}
}
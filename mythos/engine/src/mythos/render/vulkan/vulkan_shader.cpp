#include <mythos/render/vulkan/vulkan_utility.hpp>
#include <mythos/render/vulkan/vulkan_shader.hpp>

namespace myth::vulkan {
    MYL_NO_DISCARD static constexpr auto shader_type_to_VkShaderStageFlagBits(shader_type type) -> VkShaderStageFlagBits {
        switch (type) {
			using enum shader_type;
			case vertex:					return VK_SHADER_STAGE_VERTEX_BIT;
			case fragment:					return VK_SHADER_STAGE_FRAGMENT_BIT;
			case compute:					return VK_SHADER_STAGE_COMPUTE_BIT;
			case geometry:					return VK_SHADER_STAGE_GEOMETRY_BIT;
			case tessellation_control:		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			case tessellation_evaluation:	return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			case unknown:					return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
			default:						return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
		}
    }

    MYL_NO_DISCARD shader::shader(context& context,const std::unordered_map<shader_type, shader_binary_type>& binaries)
        : m_context{ context }
        , m_pipeline{ nullptr } {

        // Create shader modules

        std::unordered_map<shader_type, VkShaderModule> modules;
        for (const auto& [type, source] : binaries) {
            VkShaderModuleCreateInfo shader_module_create_info{
                .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                //.pNext = ,
                //.flags = ,
                .codeSize = source.size() * sizeof(decltype(source)::value_type),
                .pCode    = reinterpret_cast<const uint32_t*>(source.data())
            };

            MYTHOS_VULKAN_VERIFY(vkCreateShaderModule, m_context.device(), &shader_module_create_info, VK_NULL_HANDLE, &modules[type]);
        }

        // Create shader stages

        std::vector<VkPipelineShaderStageCreateInfo> shader_stages_create_info;
        shader_stages_create_info.reserve(modules.size());
        for (const auto& [type, module] : modules)
            shader_stages_create_info.emplace_back(VkPipelineShaderStageCreateInfo{
                .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                //.pNext = ,
                //.flags = ,
                .stage  = shader_type_to_VkShaderStageFlagBits(type),
                .module = module,
                .pName = "main",
                //.pSpecializationInfo = 
            });

        // Create pipeline

        m_pipeline = std::make_unique<vulkan::pipeline>(m_context);

        // Destroy shader stages
        // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkDestroyShaderModule.html
        // "A shader module can be destroyed while pipelines created using its shaders are still in use."

        for (auto& [type, module] : modules)
            if (module != VK_NULL_HANDLE)
                vkDestroyShaderModule(m_context.device(), module, VK_NULL_HANDLE);
    }

    shader::~shader() {
        m_pipeline.reset();
    }
}

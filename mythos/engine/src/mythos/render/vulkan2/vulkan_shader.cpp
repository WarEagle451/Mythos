#include <mythos/render/vulkan/vulkan_utility.hpp>
#include <mythos/render/vulkan2/vulkan_shader.hpp>

#include <unordered_map>

namespace myth::vulkan2 {
    MYL_NO_DISCARD static constexpr auto shader_type_to_VkShaderStageFlagBits(shader_type type) -> VkShaderStageFlagBits {
        switch (type) {
            using enum shader_type;
            case vertex:                  return VK_SHADER_STAGE_VERTEX_BIT;
            case fragment:                return VK_SHADER_STAGE_FRAGMENT_BIT;
            case compute:                 return VK_SHADER_STAGE_COMPUTE_BIT;
            case geometry:                return VK_SHADER_STAGE_GEOMETRY_BIT;
            case tessellation_control:    return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            case tessellation_evaluation: return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            case unknown:                 return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
            default:                      return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
        }
    }

    MYL_NO_DISCARD static constexpr auto shader_primitive_to_VkPrimitiveTopology(shader_primitive primitive) -> VkPrimitiveTopology {
        switch (primitive) {
            using enum shader_primitive;
            case point:    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            case line:     return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case triangle: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            default:       return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
        }
    }

    auto shader::create(shader* h, device& device, const create_info& ci, VkAllocationCallbacks* allocator) -> void {
        // Create shader modules

        std::unordered_map<shader_type, VkShaderModule> modules;
        for (const auto& [type, source] : ci.binaries) {
            VkShaderModuleCreateInfo shader_module_create_info{
                .sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
                //.pNext = ,
                //.flags = ,
                .codeSize = static_cast<uint32_t>(source.size() * sizeof(decltype(source)::value_type)),
                .pCode    = reinterpret_cast<const uint32_t*>(source.data())
            };

            MYTHOS_VULKAN_VERIFY(vkCreateShaderModule, device.logical(), &shader_module_create_info, VK_NULL_HANDLE, &modules[type]);
        }

        // Create shader stages

        std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos;
        shader_stage_create_infos.reserve(modules.size());
        for (const auto& [type, module] : modules)
            shader_stage_create_infos.emplace_back(VkPipelineShaderStageCreateInfo{
                .sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                //.pNext = ,
                //.flags = ,
                .stage  = shader_type_to_VkShaderStageFlagBits(type),
                .module = module,
                .pName  = "main",
                //.pSpecializationInfo = 
            });

        // Other

        /// MYTODO: scissor and viewport are the exact same in the vulkan backend begin_frame, should this be a shared object?

        VkRect2D scissor{
            .offset = { .x = 0, .y = 0 },
            .extent = ci.swapchain_extent
        };

        VkViewport viewport{
            .x        = 0.f,
            .y        = 0.f,
            .width    = static_cast<float>(scissor.extent.width),
            .height   = static_cast<float>(scissor.extent.height),
            .minDepth = 0.f,
            .maxDepth = 1.f
        };

        // Create pipeline

        pipeline::create_info pipeline_create_info{
            .primitive                 = shader_primitive_to_VkPrimitiveTopology(ci.primitive),
            .scissor                   = scissor,
            .viewport                  = viewport,
            .shader_stage_create_infos = shader_stage_create_infos,
            .render_pass               = ci.render_pass
        };

        pipeline::create(&h->m_pipeline, device, pipeline_create_info, allocator);

        // Destroy shader stages
        // https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/vkDestroyShaderModule.html
        // "A shader module can be destroyed while pipelines created using its shaders are still in use."

        for (auto& [type, module] : modules)
            if (module != VK_NULL_HANDLE)
                vkDestroyShaderModule(device.logical(), module, allocator);
    }

    auto shader::destroy(shader* h, device& device, VkAllocationCallbacks* allocator) noexcept -> void {
        pipeline::destroy(&h->m_pipeline, device, allocator);
    }

    auto shader::bind(VkCommandBuffer command_buffer) -> void {
        m_pipeline.bind(command_buffer);
    }
}

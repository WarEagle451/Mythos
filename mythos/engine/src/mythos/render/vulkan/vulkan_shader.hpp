#pragma once
#include <mythos/render/shader.hpp>
#include <mythos/render/vulkan/vulkan_pipeline.hpp>
#include <mythos/render/vulkan/vulkan_swapchain.hpp>

#include <memory>
#include <unordered_map>

namespace myth::vulkan {
    class shader : public myth::shader {
        context& m_context;

        std::unique_ptr<pipeline> m_pipeline;
    public:
        MYL_NO_DISCARD shader(context& context, const swapchain& swapchain, const std::unordered_map<shader_type, shader_binary_type>& binaries, const shader_primitive primitive);
        ~shader();
    };
}

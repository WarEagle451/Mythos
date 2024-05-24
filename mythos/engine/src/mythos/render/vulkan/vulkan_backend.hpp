#pragma once
#include <mythos/render/renderer_backend.hpp>
#include <mythos/render/vulkan/vulkan_context.hpp>
#include <mythos/render/vulkan/vulkan_swapchain.hpp>

namespace myth::vulkan {
    class backend : public myth::renderer_backend {
        context m_context;
        swapchain m_swapchain;
    public:
        MYL_NO_DISCARD backend(const renderer_configuration& config);
        ~backend();

        auto create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, shader_primitive primitive) -> std::unique_ptr<myth::shader> override;

        auto on_window_resize(const myl::i32vec2& dimensions) -> void override;
    };
}

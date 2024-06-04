#pragma once
#include <mythos/render/renderer_backend.hpp>
#include <mythos/render/vulkan/vulkan_context.hpp>
#include <mythos/render/vulkan/vulkan_swapchain.hpp>
#include <mythos/render/vulkan/vulkan_render_pass.hpp>

namespace myth::vulkan {
    class backend : public myth::renderer_backend {
        VkExtent2D m_cached_framebuffer_size;

        context m_context;
        swapchain m_swapchain;
        render_pass m_main_render_pass;

        bool m_framebuffer_resized;
    public:
        MYL_NO_DISCARD backend(const renderer_configuration& config);
        ~backend();

        auto create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, shader_primitive primitive) -> std::unique_ptr<myth::shader> override;

        auto prepare_shutdown() -> void override;

        auto draw(myth::shader& shader) -> void override;

        auto begin_frame() -> bool override;
        auto end_frame() -> void override;

        auto on_window_resize(const myl::i32vec2& dimensions) -> void override;
    };
}

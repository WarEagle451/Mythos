#include <mythos/core/application.hpp>
#include <mythos/render/vulkan/vulkan_backend.hpp>
#include <mythos/render/vulkan/vulkan_shader.hpp>

/// MYTODO: Instead of calling code like below have a second struct that. Call is renderer_create_info

namespace myth::vulkan {
    MYL_NO_DISCARD backend::backend(const renderer_configuration& config)
        : m_context(*application::get().main_window())
        , m_swapchain(m_context, *application::get().main_window())
        , m_main_render_pass(m_context, m_swapchain.image_format()) {
        m_swapchain.recreate_framebuffers(m_main_render_pass);
    }

    backend::~backend() {
        /// Is this necessary?
        // Framebuffers should be destroyed before the image views and render pass they are based on
        m_swapchain.destroy_framebuffers();
    }

    auto backend::create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, shader_primitive primitive) -> std::unique_ptr<myth::shader> {
        return std::make_unique<vulkan::shader>(m_context, m_main_render_pass, m_swapchain, shader_binaries, primitive);
    }

    auto backend::on_window_resize(const myl::i32vec2& dimensions) -> void {
        ///m_swapchain.recreate(dimensions);
    }
}

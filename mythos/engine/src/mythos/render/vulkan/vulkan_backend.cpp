#include <mythos/core/application.hpp>
#include <mythos/render/vulkan/vulkan_backend.hpp>
#include <mythos/render/vulkan/vulkan_shader.hpp>

/// MYTODO: Instead of calling code like below have a second struct that. Call is renderer_create_info

namespace myth::vulkan {
    MYL_NO_DISCARD backend::backend(const renderer_configuration& config)
        : m_context(*application::get().main_window())
        , m_swapchain(m_context, *application::get().main_window())
        , m_main_render_pass(m_context, m_swapchain.image_format()) {
        m_main_render_pass.set_clear_color(VkClearColorValue{ config.clear_color.r, config.clear_color.g, config.clear_color.b, 1.f });

        m_swapchain.recreate_framebuffers(m_main_render_pass);
        m_context.create_command_buffers(m_swapchain);
    }

    backend::~backend() {
        /// Is this necessary?
        // Framebuffers should be destroyed before the image views and render pass they are based on
        m_swapchain.destroy_framebuffers();
    }

    auto backend::create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, shader_primitive primitive) -> std::unique_ptr<myth::shader> {
        return std::make_unique<vulkan::shader>(m_context, m_swapchain, m_main_render_pass.handle(), shader_binaries, primitive);
    }

    auto backend::begin_frame() -> bool {
        /// MYTODO: scissor and viewport are the exact same in the vulkan shader constructor, should this be a shared object?

        ///bool swapchain_good = m_swapchain.acquire_next_image();
        ///if (!swapchain_good)
        ///    return false;

        VkRect2D scissor{
            .offset = { .x = 0, .y = 0 },
            .extent = m_swapchain.extent()
        };

        VkViewport viewport{
            .x        = 0.f,
            .y        = static_cast<float>(scissor.extent.height), /// MYTODO: tutorial has this as 0.f
            .width    = static_cast<float>(scissor.extent.width),
            .height   = -static_cast<float>(scissor.extent.height),
            .minDepth = 0.f,
            .maxDepth = 1.f
        };

        auto current_image_index = m_swapchain.current_image_index();
        command_buffer& current_command_buffer = m_context.command_buffers()[current_image_index];
        current_command_buffer.reset();
        current_command_buffer.begin();

        // Begin render pass
        m_main_render_pass.begin(current_command_buffer.handle(), m_swapchain.framebuffers()[current_image_index], scissor);

        // Record commands

        vkCmdSetViewport(current_command_buffer.handle(), 0, 1, &viewport);
        vkCmdSetScissor(current_command_buffer.handle(), 0, 1, &scissor);

        return true;
    }

    auto backend::end_frame() -> void {
        auto current_image_index = m_swapchain.current_image_index();
        command_buffer& current_command_buffer = m_context.command_buffers()[current_image_index];

        m_main_render_pass.end(current_command_buffer.handle());
        current_command_buffer.end();
    }

    auto backend::draw() -> void {
        auto current_image_index = m_swapchain.current_image_index();
        command_buffer& current_command_buffer = m_context.command_buffers()[current_image_index];

        /// MYTEMP: Replace with proper draw commands
        vkCmdDraw(current_command_buffer.handle(), 3, 1, 0, 0);
    }

    auto backend::on_window_resize(const myl::i32vec2& dimensions) -> void {
        ///m_swapchain.recreate(dimensions);
    }
}

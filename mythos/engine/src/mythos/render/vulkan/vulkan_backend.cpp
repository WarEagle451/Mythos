#include <mythos/core/application.hpp>
#include <mythos/render/vulkan/vulkan_backend.hpp>
#include <mythos/render/vulkan/vulkan_shader.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>

/// MYTODO: Instead of calling code like below have a second struct that. Call is renderer_create_info

namespace myth::vulkan {
    MYL_NO_DISCARD backend::backend(const renderer_configuration& config)
        : m_context(*application::get().main_window())
        , m_swapchain(m_context, *application::get().main_window())
        , m_main_render_pass(m_context, m_swapchain.image_format()) {
        m_main_render_pass.set_clear_color(VkClearColorValue{ config.clear_color.r, config.clear_color.g, config.clear_color.b, 1.f });

        m_swapchain.recreate_framebuffers(m_main_render_pass);
        m_context.create_command_buffers(m_swapchain.max_frames_in_flight());
    }

    backend::~backend() {
        /// Is this necessary?
        // Framebuffers should be destroyed before the image views and render pass they are based on
        m_swapchain.destroy_framebuffers();

        vkDeviceWaitIdle(m_context.device());
    }

    auto backend::create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, shader_primitive primitive) -> std::unique_ptr<myth::shader> {
        return std::make_unique<vulkan::shader>(m_context, m_swapchain, m_main_render_pass.handle(), shader_binaries, primitive);
    }

    auto backend::prepare_shutdown() -> void {
        vkDeviceWaitIdle(m_context.device());
    }

    auto backend::draw() -> void {
        uint32_t current_frame = m_swapchain.current_frame();
        command_buffer& current_command_buffer = m_context.command_buffers()[current_frame];

        /// MYTEMP: Replace with proper draw commands
        vkCmdDraw(current_command_buffer.handle(), 3, 1, 0, 0);
    }

    auto backend::begin_frame() -> bool {
        uint32_t current_frame = m_swapchain.current_frame();

        { // Wait for the previous frame to stop rendering
            VkFence current_fence = m_swapchain.fences_in_flight()[current_frame];
            vkWaitForFences(m_context.device(), 1, &current_fence, VK_TRUE, UINT64_MAX);
            vkResetFences(m_context.device(), 1, &current_fence);
        }

        const bool swapchain_good = m_swapchain.acquire_next_image();
        if (!swapchain_good)
            return false;

        /// MYTODO: scissor and viewport are the exact same in the vulkan shader constructor, should this be a shared object?

        VkRect2D scissor{
            .offset = { .x = 0, .y = 0 },
            .extent = m_swapchain.extent()
        };

        VkViewport viewport{
            .x        = 0.f,
            .y        = 0.f,
            .width    = static_cast<float>(scissor.extent.width),
            .height   = static_cast<float>(scissor.extent.height),
            .minDepth = 0.f,
            .maxDepth = 1.f
        };

        command_buffer& current_command_buffer = m_context.command_buffers()[current_frame];
        current_command_buffer.reset();
        current_command_buffer.begin();

        // Begin render pass
        m_main_render_pass.begin(current_command_buffer.handle(), m_swapchain.framebuffers()[m_swapchain.current_image_index()], scissor);

        // Record commands

        vkCmdSetViewport(current_command_buffer.handle(), 0, 1, &viewport);
        vkCmdSetScissor(current_command_buffer.handle(), 0, 1, &scissor);

        return true;
    }

    auto backend::end_frame() -> void {
        const uint32_t current_frame = m_swapchain.current_frame();
        command_buffer& current_command_buffer = m_context.command_buffers()[current_frame];

        m_main_render_pass.end(current_command_buffer.handle());
        current_command_buffer.end();

        VkSemaphore wait_semaphores[] = {
            m_swapchain.image_available_semaphores()[current_frame]
        };

        VkPipelineStageFlags wait_stages[] = {
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        };

        VkSemaphore signal_semaphores[] = {
            m_swapchain.render_finished_semaphores()[current_frame]
        };

        VkSubmitInfo submit_info{
            .sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO,
            //.pNext                = ,
            .waitSemaphoreCount   = 1,
            .pWaitSemaphores      = wait_semaphores,
            .pWaitDstStageMask    = wait_stages,
            .commandBufferCount   = 1,
            .pCommandBuffers      = &current_command_buffer.handle(),
            .signalSemaphoreCount = 1,
            .pSignalSemaphores    = signal_semaphores,
        };

        MYTHOS_VULKAN_VERIFY(vkQueueSubmit, m_context.queue_graphics(), 1, &submit_info, m_swapchain.fences_in_flight()[current_frame]);

        // Presentation

        VkSwapchainKHR swapchains[] = {
            m_swapchain.handle()
        };

        VkPresentInfoKHR present_info{
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            //.pNext              = ,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = signal_semaphores,
            .swapchainCount     = 1,
            .pSwapchains        = swapchains,
            .pImageIndices      = &m_swapchain.current_image_index(),
            .pResults           = nullptr
        };

        VkResult present_result = vkQueuePresentKHR(m_context.queue_present(), &present_info);
        m_swapchain.advance_current_frame();

        if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR)
            ; /// MYTODO: recreate the swapchain
        else if (present_result != VK_SUCCESS)
            MYTHOS_FATAL("Failed to present swapchain image: {}", vkresult_string(present_result, true));
    }

    auto backend::on_window_resize(const myl::i32vec2& dimensions) -> void {
        ///m_swapchain.recreate(dimensions);
    }
}

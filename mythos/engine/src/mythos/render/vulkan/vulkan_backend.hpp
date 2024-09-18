#pragma once
#include <mythos/render/renderer_backend.hpp>
#include <mythos/render/vulkan/vulkan_command_objects.hpp>
#include <mythos/render/vulkan/vulkan_device.hpp>
#include <mythos/render/vulkan/vulkan_render_pass.hpp>
#include <mythos/render/vulkan/vulkan_swapchain.hpp>

/// MYTODO: Maybe take all the sync objects out of swapchain and command buffers out of backend and put them into a class called frame.
/// Take m_current_frame_index out of swapchain and put into backend

/// MYTODO: THIS MAY BE RELEVENT TO PREVENT DRAW CALLS FROM OVERWRITING EACHOTHER
/// - https://vulkan-tutorial.com/Vertex_buffers/Vertex_input_description#comment-4656925945

#ifdef MYL_DEBUG /// MYTEMP: MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS should be passed on the command line
#   define MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
#endif

namespace myth::vulkan {
    class backend : public myth::renderer_backend {
        VkInstance   m_instance;
        VkSurfaceKHR m_surface;
        
        device       m_device;
        command_pool m_graphics_command_pool;
        swapchain    m_swapchain;
        render_pass  m_main_render_pass;

        std::vector<command_buffer> m_graphics_command_buffers;

        uint32_t m_current_frame_index;

        bool       m_framebuffer_resized;
        VkExtent2D m_cached_framebuffer_extent;

        bool m_cached_vsync;
        bool m_vsync;

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
    public:
        MYL_NO_DISCARD backend(const renderer_configuration& config);
        ~backend();

        MYL_NO_DISCARD auto create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, const shader_layout& layout, shader_primitive primitive) -> std::unique_ptr<myth::shader> override;
        auto destroy_shader(myth::shader* shader) -> void override;

        MYL_NO_DISCARD auto create_render_buffer(render_buffer_usage usage, myl::usize bytes) -> std::unique_ptr<myth::render_buffer> override;
        auto destroy_render_buffer(myth::render_buffer* buffer) -> void override;

        auto set_clear_color(const myl::f32vec3& color) -> void override;
        auto set_vsync(bool value) -> void override;

        auto begin_frame(frame_data* frame_data) -> void override;
        auto end_frame() -> void override;

        auto draw(indexed_draw_data& draw_data) -> void override;

        auto prepare_shutdown() -> void override;

        auto on_window_resize(const myl::i32vec2& dimensions) -> void override;
    private:
        auto create_instance() -> void;
        auto destroy_instance() -> void;
    };
}

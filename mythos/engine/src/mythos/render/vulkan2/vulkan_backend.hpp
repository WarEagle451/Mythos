#pragma once
#include <mythos/render/renderer_backend.hpp>
#include <mythos/render/vulkan2/vulkan_device.hpp>
#include <mythos/render/vulkan2/vulkan_render_pass.hpp>
#include <mythos/render/vulkan2/vulkan_swapchain.hpp>

#ifdef MYL_DEBUG /// MYTEMP: MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS should be passed on the command line
#   define MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
#endif

namespace myth::vulkan2 {
    class backend : public myth::renderer_backend {
        VkInstance m_instance;
        VkSurfaceKHR m_surface;
        
        device m_device;
        swapchain m_swapchain;
        render_pass m_main_render_pass;

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
    public:
        MYL_NO_DISCARD backend(const renderer_configuration& config);
        ~backend();

        MYL_NO_DISCARD auto create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, shader_primitive primitive) -> std::unique_ptr<myth::shader> override;
        auto destroy_shader(myth::shader* shader) -> void override;
    private:
        auto create_instance() -> void;
        auto destroy_instance() -> void;
    public: /// MYTODO: Remove below
        auto prepare_shutdown() -> void override {}
        auto draw() -> void override {}
        auto begin_frame() -> bool override { return true; }
        auto end_frame() -> void override {}
        auto on_window_resize(const myl::i32vec2& dimensions) -> void override {}
    };
}

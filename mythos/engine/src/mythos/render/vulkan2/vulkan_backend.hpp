#pragma once
#include <mythos/render/renderer_backend.hpp>
#include <mythos/render/vulkan2/vulkan_device.hpp>

#ifdef MYL_DEBUG /// MYTEMP: MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS should be passed on the command line
#   define MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
#endif

namespace myth::vulkan2 {
    class backend : public myth::renderer_backend {
        VkInstance m_instance;
        VkSurfaceKHR m_surface;
        
        device m_device;

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
    public:
        MYL_NO_DISCARD backend(const renderer_configuration& config);
        ~backend();
    private:
        auto create_instance() -> void;

        auto destroy_instance() -> void;
    public: /// MYTODO: Remove below
        auto create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, shader_primitive primitive) -> std::unique_ptr<myth::shader> override { return nullptr; }
        auto prepare_shutdown() -> void override {}
        auto draw() -> void override {}
        auto begin_frame() -> bool override { return true; }
        auto end_frame() -> void override {}
        auto on_window_resize(const myl::i32vec2& dimensions) -> void override {}
    };
}

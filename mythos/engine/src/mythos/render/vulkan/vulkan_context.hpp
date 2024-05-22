#pragma once
#include <mythos/platform/window.hpp>

#include <myl/definitions.hpp>

#include <vulkan/vulkan.h>

#include <vector>

/// MYTEMP: MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS should be passed on the command line 
#ifdef MYL_DEBUG
#   define MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
#endif

namespace myth::vulkan {
    struct physical_device_requirements {
        bool compute_queue;
        bool graphics_queue;
        bool present_queue;
        bool transfer_queue;

        bool discrete_gpu;

        std::vector<const char*> extensions;
    };

    struct device_queue_indices {
        enum {
            not_available = 65536
        };

        uint32_t compute  = not_available;
        uint32_t graphics = not_available;
        uint32_t present  = not_available;
        uint32_t transfer = not_available;
    };

    MYL_NO_DISCARD constexpr auto find_queue_family_indices(VkPhysicalDevice physical_device) -> device_queue_indices;

    class context {
        VkInstance m_instance;

        VkSurfaceKHR m_surface;

        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
        VkQueue m_queue_compute;
        VkQueue m_queue_graphics;
        VkQueue m_queue_present;
        VkQueue m_queue_transfer;

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
    public:
        MYL_NO_DISCARD context(window& window);
        ~context();

        MYL_NO_DISCARD auto physical_device() const -> VkPhysicalDevice { return m_physical_device; }
        MYL_NO_DISCARD auto device() const -> VkDevice { return m_device; }
        MYL_NO_DISCARD auto surface() const -> VkSurfaceKHR { return m_surface; }
    private:
        auto create_instance() -> void;
        auto create_surface(window& window) -> void;
        auto create_device() -> void;

        auto destroy_device() -> void;
        auto destroy_surface() -> void;
        auto destroy_instance() -> void;
    };
}

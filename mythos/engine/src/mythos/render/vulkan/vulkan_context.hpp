#pragma once
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

    class context {
        VkInstance m_instance;

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
        MYL_NO_DISCARD context();
        ~context();
    private:
        auto create_instance(std::vector<const char*>* validation_layers, std::vector<const char*>* extensions) -> void;
        auto create_device(const std::vector<const char*>& validation_layers, const std::vector<const char*>& extensions) -> void;

        auto destroy_device() -> void;
        auto destroy_instance() -> void;
    };
}

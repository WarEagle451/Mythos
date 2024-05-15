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
            index_max = 65536
        };

        uint32_t compute  = index_max;
        uint32_t graphics = index_max;
        uint32_t present  = index_max;
        uint32_t transfer = index_max;
    };

    class context {
        VkInstance m_instance;

        VkPhysicalDevice m_device;

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
    public:
        MYL_NO_DISCARD context();
        ~context();
    private:
        auto create_instance() -> void;
        auto create_device() -> void;

        auto destroy_instance() -> void;
    };
}

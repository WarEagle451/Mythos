#pragma once
#include <myl/definitions.hpp>

#include <vulkan/vulkan.h>

/// MYTEMP: MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS should be passed on the command line 
#ifdef MYL_DEBUG
#   define MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
#endif

namespace myth::vulkan {
    class context {
        VkInstance m_instance;

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
    public:
        MYL_NO_DISCARD context();
        ~context();
    private:
        auto create_instance() -> void;

        auto destroy_instance() -> void;
    };
}

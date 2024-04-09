#include <mythos/core/application.hpp>
#include <mythos/render/vulkan/vulkan_context.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>
#include <mythos/version.hpp>

#include <cstring>

// Continue from: https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Physical_devices_and_queue_families

namespace myth::vulkan {
#ifdef VK_EXT_DEBUG_UTILS_EXTENSION_NAME
    static VKAPI_ATTR auto debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) -> VkBool32 {
        /// MYTODO: Extend Vulkan debug callback with the following:
        /// - type
        ///    - VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT - Ignore
        ///    - VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT - Violates spec or possible mistake
        ///    - VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT - Possible non-optimal use of Vulkan
        /// - callback_data
        ///    - pObjects - Array of Vulkan objects related to the message
        ///    - objectCount - Number of objects
        
        switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: MYTHOS_TRACE("Validation Layer: {}", callback_data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    MYTHOS_INFO("Validation Layer: {}", callback_data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: MYTHOS_WARN("Validation Layer: {}", callback_data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   MYTHOS_ERROR("Validation Layer: {}", callback_data->pMessage); break;
            default:                                              MYTHOS_ERROR("Validation Layer: {}", callback_data->pMessage); break;
        }
        return VK_FALSE;
    }
#endif

    static auto get_required_extensions(std::vector<const char*>* required) -> void {
        get_platform_required_extensions(required);

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        required->emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

#ifdef MYL_DEBUG
        if (!required->empty()) {
            MYTHOS_DEBUG("Required Vulkan extensions:");
            for (const auto& str : *required)
                MYTHOS_DEBUG("\t- {}", str);
        }

        uint32_t available_count = 0;
        vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &available_count, VK_NULL_HANDLE);
        std::vector<VkExtensionProperties> available(available_count);
        vkEnumerateInstanceExtensionProperties(VK_NULL_HANDLE, &available_count, available.data());

        if (!available.empty()) {
            MYTHOS_DEBUG("Available Vulkan extensions:");
            for (const auto& ex : available)
                MYTHOS_DEBUG("\t- {}", ex.extensionName);
        }
#endif
    }

    static auto get_required_validation_layers(std::vector<const char*>* required) -> void {
#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        required->emplace_back("VK_LAYER_KHRONOS_validation");

        uint32_t count = 0;
        vkEnumerateInstanceLayerProperties(&count, VK_NULL_HANDLE);
        std::vector<VkLayerProperties> available(count);
        vkEnumerateInstanceLayerProperties(&count, available.data());

#ifdef MYL_DEBUG
        if (!required->empty()) {
            MYTHOS_DEBUG("Required Vulkan validation layers:");
            for (const auto& str : *required)
                MYTHOS_DEBUG("\t- {}", str);
        }

        if (!available.empty()) {
            MYTHOS_DEBUG("Available Vulkan validation layers:");
            for (const auto& l : available)
                MYTHOS_DEBUG("\t- {}", l.layerName);
        }
#endif

        for (const auto& r : *required) {
            bool found = false;
            for (const auto& a : available)
                if (strcmp(r, a.layerName) == 0) {
                    found = true;
                    break;
                }

            if (!found)
                MYTHOS_FATAL("Required validation layer '{}' not found", r);
        }
#endif
    }

    MYL_NO_DISCARD context::context() {
        create_instance();
    }

    context::~context() {
        destroy_instance();
    }

    auto context::create_instance() -> void {
        const application_information& mythos_app_info = application::get().info();
        VkApplicationInfo application_info{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = mythos_app_info.name,
            .applicationVersion = VK_MAKE_API_VERSION(0, mythos_app_info.version_major, mythos_app_info.version_minor, mythos_app_info.version_patch),
            .pEngineName = MYTHOS_ENGINE_NAME,
            .engineVersion = VK_MAKE_API_VERSION(0, MYTHOS_VERSION_MAJOR, MYTHOS_VERSION_MINOR, MYTHOS_VERSION_PATCH),
            .apiVersion = VK_API_VERSION_1_3
        };

        std::vector<const char*> extensions;
        get_required_extensions(&extensions);

        std::vector<const char*> validation_layers;
        get_required_validation_layers(&validation_layers);

        VkInstanceCreateInfo instance_create_info{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = static_cast<uint32_t>(validation_layers.size()),
            .ppEnabledLayerNames = validation_layers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()
        };

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_callback,
            .pUserData = VK_NULL_HANDLE // Passing a pointer here would allow that pointer to be used in the debug callback
        };
        instance_create_info.pNext = &debug_messenger_create_info;
#endif

        MYTHOS_VULKAN_VERIFY(vkCreateInstance, &instance_create_info, VK_NULL_HANDLE, &m_instance);

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        auto vkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
        if (vkCreateDebugUtilsMessengerEXT == nullptr)
            MYTHOS_FATAL("Failed to load 'vkCreateDebugUtilsMessengerEXT'");
        MYTHOS_VULKAN_VERIFY(vkCreateDebugUtilsMessengerEXT, m_instance, &debug_messenger_create_info, VK_NULL_HANDLE, &m_debug_messenger);
#endif
    }

    auto context::destroy_instance() -> void {
#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        auto vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (vkDestroyDebugUtilsMessengerEXT == nullptr)
            MYTHOS_FATAL("Failed to load 'vkDestroyDebugUtilsMessengerEXT'");
        else
            vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, VK_NULL_HANDLE);
#endif

        vkDestroyInstance(m_instance, VK_NULL_HANDLE);
    }
}

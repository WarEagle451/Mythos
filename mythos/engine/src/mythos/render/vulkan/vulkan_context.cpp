#include <mythos/core/application.hpp>
#include <mythos/render/vulkan/vulkan_context.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>
#include <mythos/version.hpp>

#include <cstring>

// Continue from: https://vulkan-tutorial.com/en/Drawing_a_triangle/Setup/Logical_device_and_queues

/// MYTODO: Expand find_queue_family

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
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: MYTHOS_TRACE("Validation Layer - {}", callback_data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    MYTHOS_INFO("Validation Layer - {}", callback_data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: MYTHOS_WARN("Validation Layer - {}", callback_data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   MYTHOS_ERROR("Validation Layer - {}", callback_data->pMessage); break;
            default:                                              MYTHOS_ERROR("Validation Layer - {}", callback_data->pMessage); break;
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

    MYL_NO_DISCARD constexpr auto find_queue_family_indices(VkPhysicalDevice device) -> device_queue_indices {
        myl::u32 queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, VK_NULL_HANDLE);
        std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_family_properties.data());

        device_queue_indices dqi;
        for (myl::u32 i = 0; const auto& qf : queue_family_properties) {
            if (qf.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                dqi.compute = i;
            }

            if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                dqi.graphics = i;
            }

            /// MYTODO:
            /// if (qf.queueFlags & VK_QUEUE_TRANSFER_BIT) dqi.transfer = i; Best score needs to be obtained
            /// Present

            ++i;
        }

        return dqi;
    }

    MYL_NO_DISCARD static auto physical_device_meets_requirements(VkPhysicalDevice device, VkPhysicalDeviceProperties* properties, VkPhysicalDeviceFeatures* features, const physical_device_requirements& requirements) -> bool {
        vkGetPhysicalDeviceProperties(device, properties);
        vkGetPhysicalDeviceFeatures(device, features);

        if (requirements.discrete_gpu && (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU))
            return false;

        device_queue_indices dqi = find_queue_family_indices(device);
        if (requirements.compute_queue && (dqi.compute == device_queue_indices::index_max)) return false;
        if (requirements.graphics_queue && (dqi.graphics == device_queue_indices::index_max)) return false;
        if (requirements.present_queue && (dqi.present == device_queue_indices::index_max)) return false;
        if (requirements.transfer_queue && (dqi.transfer == device_queue_indices::index_max)) return false;

        return true;
    }

    MYL_NO_DISCARD static auto rate_physical_device(const VkPhysicalDeviceProperties& properties, const VkPhysicalDeviceFeatures& features) -> myl::u64 {
        myl::u64 rating = 0;
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            rating += 1000;

        // Maximum possible size of textures affects graphics quality
        rating += properties.limits.maxImageDimension2D;

        return rating;
    }

    MYL_NO_DISCARD static auto select_physical_device(const physical_device_requirements& requirements, VkInstance instance) -> VkPhysicalDevice {
        if (instance == VK_NULL_HANDLE)
            return VK_NULL_HANDLE;

        uint32_t count = 0;
        vkEnumeratePhysicalDevices(instance, &count, VK_NULL_HANDLE);
        if (count == 0) {
            MYTHOS_FATAL("Failed to locate a GPU with Vulkan support");
            return VK_NULL_HANDLE;
        }
        
        std::vector<VkPhysicalDevice> canidates(count);
        vkEnumeratePhysicalDevices(instance, &count, canidates.data());

        myl::u64 best_rating = 0;
        VkPhysicalDevice best_canidate = VK_NULL_HANDLE;
        for (const auto& device : canidates) {
            VkPhysicalDeviceProperties properties{};
            VkPhysicalDeviceFeatures features{};

            if (physical_device_meets_requirements(device, &properties, &features, requirements)) {
                // If the device meets requirements its rating is 1
                myl::u64 rating = 1 + rate_physical_device(properties, features);
                if (rating > best_rating) {
                    best_canidate = device;
                    best_rating = rating;
                }
            }
        }

        if (best_canidate == VK_NULL_HANDLE)
            MYTHOS_FATAL("No GPUs meet requirements");
        return best_canidate;
    }

    MYL_NO_DISCARD context::context() {
        create_instance();
        create_device();
    }

    context::~context() {
        // m_device will be destroyed upon destruction of m_instance
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

    auto context::create_device() -> void {
        /// MYTODO: Requirements should be user definable
        physical_device_requirements requirements{
            .compute_queue = false,
            .graphics_queue = true,
            .present_queue = false, /// MYTODO: Should be true
            .transfer_queue = false,
            .discrete_gpu = true
        };

        m_device = select_physical_device(requirements, m_instance);
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

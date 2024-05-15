#include <mythos/core/application.hpp>
#include <mythos/render/vulkan/vulkan_context.hpp>
#include <mythos/render/vulkan/vulkan_utility.hpp>
#include <mythos/version.hpp>

#include <cstring>
#include <limits>
#include <map>

// Continue from: https://vulkan-tutorial.com/Drawing_a_triangle/Presentation/Window_surface

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

    /// MYTODO: Finding queue families might be more complex

    MYL_NO_DISCARD constexpr auto find_queue_family_indices(VkPhysicalDevice physical_device) -> device_queue_indices {
        myl::u32 queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, VK_NULL_HANDLE);
        std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties.data());

        device_queue_indices dqi;
        myl::u32 best_transfer_queue_score = std::numeric_limits<myl::u32>::max(); // The queue with the lowest index increases the chance it is a dedicated transfer queue
        for (myl::u32 i = 0; const auto& qf : queue_family_properties) {
            myl::u32 current_transfer_queue_score = 0;
            if (qf.queueFlags & VK_QUEUE_COMPUTE_BIT) {
                dqi.compute = i;
                ++current_transfer_queue_score;
            }

            if (qf.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                dqi.graphics = i;
                ++current_transfer_queue_score;
            }

            if ((qf.queueFlags & VK_QUEUE_TRANSFER_BIT) && (current_transfer_queue_score < best_transfer_queue_score)) {
                dqi.transfer = i;
                best_transfer_queue_score = current_transfer_queue_score;
            }

            /// MYTODO: Present queue

            ++i;
        }

        return dqi;
    }

    MYL_NO_DISCARD static auto physical_device_meets_requirements(VkPhysicalDevice physical_device, VkPhysicalDeviceProperties* properties, VkPhysicalDeviceFeatures* features, const physical_device_requirements& requirements) -> bool {
        vkGetPhysicalDeviceProperties(physical_device, properties);
        vkGetPhysicalDeviceFeatures(physical_device, features);

        if (requirements.discrete_gpu && (properties->deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU))
            return false;

        const device_queue_indices dqi = find_queue_family_indices(physical_device);
        if (requirements.compute_queue && (dqi.compute == device_queue_indices::not_available)) return false;
        if (requirements.graphics_queue && (dqi.graphics == device_queue_indices::not_available)) return false;
        if (requirements.present_queue && (dqi.present == device_queue_indices::not_available)) return false;
        if (requirements.transfer_queue && (dqi.transfer == device_queue_indices::not_available)) return false;

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
        for (const auto& canidate : canidates) {
            VkPhysicalDeviceProperties properties{};
            VkPhysicalDeviceFeatures features{};

            if (physical_device_meets_requirements(canidate, &properties, &features, requirements)) {
                // If the device meets requirements its rating is 1
                myl::u64 rating = 1 + rate_physical_device(properties, features);
                if (rating > best_rating) {
                    best_canidate = canidate;
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
        destroy_device();
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

        get_required_validation_layers(&m_validation_layers);

        VkInstanceCreateInfo instance_create_info{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pApplicationInfo = &application_info,
            .enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size()),
            .ppEnabledLayerNames = m_validation_layers.data(),
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
        // Select a physical device

        /// MYTODO: Requirements should be user definable
        physical_device_requirements requirements{
            .compute_queue = false,
            .graphics_queue = true,
            .present_queue = false, /// MYTODO: Should be true
            .transfer_queue = false,
            .discrete_gpu = true
        };

        m_physical_device = select_physical_device(requirements, m_instance);
        if (m_physical_device == VK_NULL_HANDLE) {
            MYTHOS_FATAL("Failed to select a physical device");
            return;
        }

        // Create logical device

        const device_queue_indices dqi = find_queue_family_indices(m_physical_device);
        std::map<uint32_t, uint32_t> indices{}; /// type, count /// MYTODO: Find a better way to detect shared indices
        if (dqi.compute  != device_queue_indices::not_available) ++indices[dqi.compute];
        if (dqi.graphics != device_queue_indices::not_available) ++indices[dqi.graphics];
        if (dqi.present  != device_queue_indices::not_available) ++indices[dqi.present];
        if (dqi.transfer != device_queue_indices::not_available) ++indices[dqi.transfer];

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos{};
        queue_create_infos.reserve(indices.size());
        for (const auto& qi : indices) {
            myl::f32 queue_priority = 1.f;
            queue_create_infos.emplace_back(VkDeviceQueueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .queueFamilyIndex = qi.first,
                .queueCount = qi.second,
                .pQueuePriorities = &queue_priority
            });
        }

        VkPhysicalDeviceFeatures physical_device_features{

        };

        VkDeviceCreateInfo device_create_info{
            .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            .queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size()),
            .pQueueCreateInfos = queue_create_infos.data(),
#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
            .enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size()),
            .ppEnabledLayerNames = m_validation_layers.data(),
#else
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = VK_NULL_HANDLE,
#endif
            .enabledExtensionCount = 0, /// Should not always be zero
            ///.ppEnabledExtensionNames = ,
            .pEnabledFeatures = &physical_device_features
        };

        MYTHOS_VULKAN_VERIFY(vkCreateDevice, m_physical_device, &device_create_info, VK_NULL_HANDLE, &m_device);

        if (dqi.compute  != device_queue_indices::not_available) { vkGetDeviceQueue(m_device, dqi.compute, 0, &m_queue_compute); MYTHOS_DEBUG("Compute queue created at index {}", dqi.compute); } // 0 because we're only creating 1 queue from this family
        if (dqi.graphics != device_queue_indices::not_available) { vkGetDeviceQueue(m_device, dqi.graphics, 0, &m_queue_graphics); MYTHOS_DEBUG("Graphics queue created at index {}", dqi.graphics); }
        if (dqi.present  != device_queue_indices::not_available) { vkGetDeviceQueue(m_device, dqi.present, 0, &m_queue_present); MYTHOS_DEBUG("Present queue created at index {}", dqi.present); }
        if (dqi.transfer != device_queue_indices::not_available) { vkGetDeviceQueue(m_device, dqi.transfer, 0, &m_queue_transfer); MYTHOS_DEBUG("Transfer queue created at index {}", dqi.transfer); }
    }

    auto context::destroy_device() -> void {
        // Queues are implicitly cleaed up when the device is destroyed
        vkDestroyDevice(m_device, VK_NULL_HANDLE);
        // m_physical_device will be destroyed upon destruction of m_instance
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

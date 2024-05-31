#include <mythos/core/application.hpp>
#include <mythos/render/vulkan2/vulkan_backend.hpp>
#include <mythos/render/vulkan2/vulkan_platform.hpp>
#include <mythos/render/vulkan2/vulkan_shader.hpp>>
#include <mythos/render/vulkan/vulkan_utility.hpp>
#include <mythos/version.hpp>

namespace myth::vulkan2 {
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

    static auto get_required_extensions(std::vector<const char*>* required) -> void {
        required->emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
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

    MYL_NO_DISCARD backend::backend(const renderer_configuration& config) {
        myth::window* window = application::get().main_window();

        create_instance();
        create_surface(&m_surface, m_instance, window, VK_NULL_HANDLE);

        device::create_info device_create_info{
            .instance                     = m_instance,
            .surface                      = m_surface,
            .physical_device_requirements = { /// MYTEMP: physical_device_requirements should be user dependent
                .discrete_gpu   = true,
                .queue_compute  = false,
                .queue_graphics = true,
                .queue_present  = true,
                .queue_transfer = false,
                .extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME }
            }
        };

        device::create(&m_device, device_create_info, VK_NULL_HANDLE);

        const auto& window_extent = window->framebuffer_size();
        swapchain::create_info swapchain_create_info{
            .surface = m_surface,
            .extent  = VkExtent2D{
                .width  = static_cast<uint32_t>(window_extent.w),
                .height = static_cast<uint32_t>(window_extent.h)
            }
        };
        
        swapchain::create(&m_swapchain, m_device, swapchain_create_info, VK_NULL_HANDLE);
        
        render_pass::create_info render_pass_create_info{
            .color_format = m_swapchain.image_format().format,
            .clear_color  = VkClearColorValue{
                .float32{
                    config.clear_color.r,
                    config.clear_color.g,
                    config.clear_color.b,
                    1.f
                }
            }
        };
        
        render_pass::create(&m_main_render_pass, m_device, render_pass_create_info, VK_NULL_HANDLE);
    }

    backend::~backend() {
        vkDeviceWaitIdle(m_device.logical());

        render_pass::destroy(&m_main_render_pass, m_device, VK_NULL_HANDLE);
        swapchain::destroy(&m_swapchain, m_device, VK_NULL_HANDLE);
        device::destroy(&m_device, VK_NULL_HANDLE);

        vkDestroySurfaceKHR(m_instance, m_surface, VK_NULL_HANDLE);

        destroy_instance();
    }

    MYL_NO_DISCARD auto backend::create_shader(const std::unordered_map<shader_type, shader_binary_type>& shader_binaries, shader_primitive primitive) -> std::unique_ptr<myth::shader> {
        vulkan2::shader::create_info shader_create_info{
            .swapchain_extent = m_swapchain.image_extent(),
            .render_pass      = m_main_render_pass.handle(),
            .binaries         = shader_binaries,
            .primitive        = primitive
        };

        std::unique_ptr<vulkan2::shader> shader = std::make_unique<vulkan2::shader>();
        vulkan2::shader::create(shader.get(), m_device, shader_create_info, VK_NULL_HANDLE);
        return shader;
    }

    auto backend::destroy_shader(myth::shader* shader) -> void {
        vulkan2::shader* vks = static_cast<vulkan2::shader*>(shader);
        vulkan2::shader::destroy(vks, m_device, VK_NULL_HANDLE);
    }

    auto backend::create_instance() -> void {
        const application_information& mythos_app_info = application::get().info();
        VkApplicationInfo application_info{
            .sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            //.pNext = ,
            .pApplicationName   = mythos_app_info.name,
            .applicationVersion = VK_MAKE_API_VERSION(0, mythos_app_info.version_major, mythos_app_info.version_minor, mythos_app_info.version_patch),
            .pEngineName        = MYTHOS_ENGINE_NAME,
            .engineVersion      = VK_MAKE_API_VERSION(0, MYTHOS_VERSION_MAJOR, MYTHOS_VERSION_MINOR, MYTHOS_VERSION_PATCH),
            .apiVersion         = VK_API_VERSION_1_3
        };

        std::vector<const char*> validation_layers;
        get_required_validation_layers(&validation_layers);

        std::vector<const char*> extensions;
        get_required_extensions(&extensions);

        VkInstanceCreateInfo instance_create_info{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            //.pNext = ,
            //.flags = ,
            .pApplicationInfo        = &application_info,
            .enabledLayerCount       = static_cast<uint32_t>(validation_layers.size()),
            .ppEnabledLayerNames     = validation_layers.data(),
            .enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()
        };

#ifdef MYTHOS_VULKAN_ENABLE_VALIDATION_LAYERS
        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_create_info{
            .sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            //.pNext = ,
            //.flags = ,
            .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback = debug_callback,
            .pUserData       = VK_NULL_HANDLE // Passing a pointer here would allow that pointer to be used in the debug callback
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

    auto backend::destroy_instance() -> void {
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

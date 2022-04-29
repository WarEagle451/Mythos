#include "vulkan_context.hpp"
#include "vulkan_platform.hpp"

#include <mythos/core/assert.hpp>
#include <mythos/core/except.hpp>

#include <string.h>

namespace myl::vulkan {
	static std::vector<const char*> get_required_extensions() {
		std::vector<const char*> extensions{};
		platform_required_extension_names(&extensions);
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME); // generic surface extension
#ifdef MYL_BUILD_DEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		MYL_CORE_DEBUG("Required Vulkan extensions:");
		for (auto name : extensions)
			MYL_CORE_DEBUG("\t- {}", name);
#endif
		return extensions;
	}

	static std::vector<const char*> get_required_validation_layers() {
		std::vector<const char*> required_layers{};
#ifdef MYL_BUILD_DEBUG
		MYL_CORE_INFO("Validation layers enabled");

		// required validation layers
		required_layers.push_back("VK_LAYER_KHRONOS_validation");

		u32 available_layer_count{};
		MYL_CORE_ASSERT(vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr) == VK_SUCCESS);
		std::vector<VkLayerProperties> available_layers(available_layer_count);
		MYL_CORE_ASSERT(vkEnumerateInstanceLayerProperties(&available_layer_count, available_layers.data()) == VK_SUCCESS);

		MYL_CORE_DEBUG("{} available validation layers:", available_layer_count);
		for (auto& layer : available_layers)
			MYL_CORE_DEBUG("\t- {}", layer.layerName);

		// verify all required layers are available
		for (auto name : required_layers) {
			bool found = false;
			for (auto& layer : available_layers)
				if (strcmp(name, layer.layerName) == 0) {
					found = true;
					break;
				}

			if (!found)
				MYL_CORE_FATAL("Required validation layer '{}' not found", name);
		}
#endif
		return required_layers;
	}

#ifdef MYL_BUILD_DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
		switch (severity) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: MYL_CORE_TRACE("Vulkan - {}", callback_data->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: MYL_CORE_INFO("Vulkan - {}", callback_data->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: MYL_CORE_WARN("Vulkan - {}", callback_data->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: MYL_CORE_ERROR("Vulkan - {}", callback_data->pMessage); break;
			default: MYL_CORE_DEBUG("Vulkan - {}", callback_data->pMessage); break;
		}

		return VK_FALSE;
	}
#endif
	context::context(const app::info& a_info) {
		// VkInstance setup
		VkApplicationInfo app_info{};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.apiVersion = VK_API_VERSION_1_3;
		app_info.pApplicationName = a_info.name.c_str(); /// MYTodo: instead of passing everything through, have a function set_app_info, this is be optional, to be done before creating app and before pushing layer
		app_info.applicationVersion = VK_MAKE_VERSION(a_info.major, a_info.minor, a_info.patch);
		app_info.pEngineName = "Mythos Engine";
		app_info.engineVersion = VK_MAKE_VERSION(MYL_VERSION_MAJOR, MYL_VERSION_MINOR, MYL_VERSION_PATCH);

		auto required_extensions = get_required_extensions();
		auto required_validation_layers = get_required_validation_layers();

		VkInstanceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info; // safe because when VkInstance is created &app_info is not needed
		create_info.enabledExtensionCount = static_cast<u32>(required_extensions.size());
		create_info.ppEnabledExtensionNames = required_extensions.data();
		create_info.enabledLayerCount = static_cast<u32>(required_validation_layers.size());
		create_info.ppEnabledLayerNames = required_validation_layers.data();

		// debugger
#ifdef MYL_BUILD_DEBUG
		MYL_CORE_DEBUG("Creating Vulkan debug messenger");
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
		debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;// | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
		debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debug_create_info.pfnUserCallback = debug_callback;
		debug_create_info.pUserData = nullptr;  // Optional

		create_info.pNext = &debug_create_info; // this will give messages during vkCreateInstance
#endif
		if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
			throw core_runtime_error("Failed to create Vulkan instance");
		MYL_CORE_INFO("Vulkan instance created");
		// creating the debugger
#ifdef MYL_BUILD_DEBUG
		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
		MYL_CORE_ASSERT(func, "Failed to create debug messenger");
		if (func(m_instance, &debug_create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
			throw core_runtime_error("Failed to create debug messenger");
#endif
		m_surface = platform_create_surface(m_instance);
		m_device = std::make_unique<vulkan::device>(*this);
		/// MYTodo: command pool
	}

	context::~context() {
		m_device.reset(); // must destory in opposite order of creation
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
#ifdef MYL_BUILD_DEBUG
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
		func(m_instance, m_debug_messenger, nullptr);
#endif
		vkDestroyInstance(m_instance, nullptr);
	}
}
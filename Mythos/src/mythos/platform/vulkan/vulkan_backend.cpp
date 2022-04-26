#include "vulkan_backend.hpp"
#include "vulkan_platform.hpp"

#include <mythos/core/log.hpp>
#include <mythos/core/assert.hpp>

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
		required_layers.push_back("VK_LAYER_KHRONOS_validation"); /// MYBug: why is this not here

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

			found ?
				MYL_CORE_INFO("Found validation layer '{}'", name) :
				MYL_CORE_FATAL("Required validation layer '{}' not found", name);
		}
#endif
		return required_layers;
	}

#ifdef MYL_BUILD_DEBUG
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback_data, void* user_data) {
		switch (severity) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: MYL_CORE_TRACE("validation layer: {}", callback_data->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: MYL_CORE_INFO("validation layer: {}", callback_data->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: MYL_CORE_WARN("validation layer: {}", callback_data->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: MYL_CORE_ERROR("validation layer: {}", callback_data->pMessage); break;
			default: MYL_CORE_DEBUG("validation layer: {}", callback_data->pMessage); break;
		}

		return VK_FALSE;
	}
#endif

	backend::backend() 
		: m_context() {
		// VkInstance setup
		VkApplicationInfo app_info{};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.apiVersion = VK_API_VERSION_1_3;
		///MYTodo: app_info.pApplicationName = ;
		///MYTodo: app_info.applicationVersion = VK_MAKE_VERSION();
		app_info.pEngineName = "Mythos Engine";
		app_info.engineVersion = VK_MAKE_VERSION(MYL_VERSION_MAJOR, MYL_VERSION_MINOR, MYL_VERSION_PATCH);

		auto required_extensions = get_required_extensions();
		auto required_validation_layers = get_required_validation_layers();

		VkInstanceCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		create_info.pApplicationInfo = &app_info; // safe because when VkInstance is created &app_info is not needed
		create_info.enabledExtensionCount = required_extensions.size();
		create_info.ppEnabledExtensionNames = required_extensions.data();
		create_info.enabledLayerCount = required_validation_layers.size();
		create_info.ppEnabledLayerNames = required_validation_layers.data();

		MYL_CORE_ASSERT(vkCreateInstance(&create_info, nullptr, &m_context.instance()) == VK_SUCCESS);
		MYL_CORE_INFO("Vulkan instance created");

		// debugger
#ifdef MYL_BUILD_DEBUG
		MYL_CORE_DEBUG("Creating Vulkan debug messenger");
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
		debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;// | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
		debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debug_create_info.pfnUserCallback = debug_callback;
		debug_create_info.pUserData = nullptr;  // Optional

		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_context.instance(), "vkCreateDebugUtilsMessengerEXT");
		MYL_CORE_ASSERT(func, "Failed to create debug messenger");
		MYL_CORE_ASSERT(func(m_context.instance(), &debug_create_info, nullptr, &m_context.debug_messenger()) == VK_SUCCESS)
#endif
		MYL_CORE_INFO("vulkan backend initialized");
	}

	backend::~backend() {
		MYL_CORE_DEBUG("Destorying Vulkan backend");
#ifdef MYL_BUILD_DEBUG
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_context.instance(), "vkCreateDebugUtilsMessengerEXT");
		func(m_context.instance(), m_context.debug_messenger(), nullptr);
#endif
		vkDestroyInstance(m_context.instance(), nullptr); /// MYBug: why is instance null?
	}

	bool backend::begin() {
		return true;
	}

	void backend::end() {

	}

	void backend::on_window_resize(u32 a_width, u32 a_height) {

	}
}
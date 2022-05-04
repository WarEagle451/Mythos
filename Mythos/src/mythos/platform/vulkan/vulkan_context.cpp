#include "vulkan_context.hpp"
#include "vulkan_platform.hpp"
#include "vulkan_common.hpp"

#include <mythos/core/except.hpp>

#include <string.h>

namespace myl::vulkan {
	static MYL_NO_DISCARD std::vector<const char*> get_required_extensions() {
		std::vector<const char*> extensions;
		platform_required_extension_names(&extensions);
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME); // generic surface extension
#ifdef MYL_BUILD_DEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		MYL_CORE_DEBUG("Required Vulkan extensions:");
		for (auto& name : extensions)
			MYL_CORE_DEBUG("\t- {}", name);
#endif
		return extensions;
	}

	static MYL_NO_DISCARD std::vector<const char*> get_required_validation_layers() {
		std::vector<const char*> required_layers{};
#ifdef MYL_BUILD_DEBUG
		MYL_CORE_INFO("Validation layers enabled");

		// required validation layers
		required_layers.push_back("VK_LAYER_KHRONOS_validation");

		u32 available_layer_count = 0;
		MYL_VK_CHECK(vkEnumerateInstanceLayerProperties, &available_layer_count, nullptr);
		std::vector<VkLayerProperties> available_layers(available_layer_count);
		MYL_VK_CHECK(vkEnumerateInstanceLayerProperties, &available_layer_count, available_layers.data());

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
		VkApplicationInfo app_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = a_info.name.c_str(), /// MYTodo: instead of passing everything through, have a function set_app_info, this is be optional, to be done before creating app and before pushing layer
			.applicationVersion = VK_MAKE_VERSION(a_info.major, a_info.minor, a_info.patch),
			.pEngineName = "Mythos Engine",
			.engineVersion = VK_MAKE_VERSION(MYL_VERSION_MAJOR, MYL_VERSION_MINOR, MYL_VERSION_PATCH),
			.apiVersion = VK_API_VERSION_1_3
		};

		auto required_extensions = get_required_extensions();
		auto required_validation_layers = get_required_validation_layers();

		VkInstanceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &app_info,
			.enabledLayerCount = static_cast<u32>(required_validation_layers.size()),
			.ppEnabledLayerNames = required_validation_layers.data(),
			.enabledExtensionCount = static_cast<u32>(required_extensions.size()),
			.ppEnabledExtensionNames = required_extensions.data()
		};

#ifdef MYL_BUILD_DEBUG // debugger
		MYL_CORE_DEBUG("Creating Vulkan debug messenger");
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT, // | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = debug_callback,
			.pUserData = nullptr  // Optional
		};
		create_info.pNext = &debug_create_info; // this will give messages during vkCreateInstance
#endif
		if (vkCreateInstance(&create_info, nullptr, &m_instance) != VK_SUCCESS)
			throw core_runtime_error("Failed to create Vulkan instance");
		MYL_CORE_INFO("Vulkan instance created");
#ifdef MYL_BUILD_DEBUG // creating the debugger
		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
		MYL_CORE_ASSERT(func, "Failed to create debug messenger");
		if (func(m_instance, &debug_create_info, nullptr, &m_debug_messenger) != VK_SUCCESS)
			throw core_runtime_error("Failed to create debug messenger");
#endif
		m_surface = platform_create_surface(m_instance);
		MYL_CORE_INFO("Vulkan surface created");
		m_device = std::make_unique<vulkan::device>(*this);
		m_swapchain = std::make_unique<vulkan::swapchain>(*this, m_framebuffer_width, m_framebuffer_height);
		MYL_CORE_INFO("Vulkan swapchain created");

		/// MYTodo: {} should work, shouldn't have to do f32vec4{}
		m_main_render_pass = std::make_unique<render_pass>(*this, 0.f, 0.f, static_cast<f32>(m_framebuffer_width), static_cast<f32>(m_framebuffer_height), f32vec4{ .1f, .1f, .1f, 1.f }, 1.f, 0);
		MYL_CORE_INFO("Main Vulkan render pass created");
		create_command_buffers();
	}

	context::~context() { // must destory in opposite order of creation
		for (auto& buffer : m_graphics_command_buffers)
			if (buffer.handle() == nullptr)
				buffer.deallocate();
		m_graphics_command_buffers.clear();
		MYL_CORE_INFO("Vulkan command buffers destroyed");

		m_main_render_pass.reset();
		MYL_CORE_INFO("Main Vulkan render pass destroyed");
		m_swapchain.reset();
		MYL_CORE_INFO("Vulkan swapchain destroyed");
		m_device.reset();
		if (m_surface)
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		MYL_CORE_INFO("Vulkan surface destroyed");
#ifdef MYL_BUILD_DEBUG
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
		func(m_instance, m_debug_messenger, nullptr);
		MYL_CORE_DEBUG("Vulkan debug messenger destroyed");
#endif
		vkDestroyInstance(m_instance, nullptr);
		MYL_CORE_INFO("Vulkan instance destroyed");
	}

	i32 context::find_memory_index(u32 a_type_filter, u32 a_property_flags) {
		VkPhysicalDeviceMemoryProperties mem_properties{};
		vkGetPhysicalDeviceMemoryProperties(m_device->physical(), &mem_properties);

		for (u32 i = 0; i != mem_properties.memoryTypeCount; ++i)
			// check each memory type to see if its bit is set
			if ((a_type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & a_property_flags) == a_property_flags)
				return i;

		MYL_CORE_WARN("Unable to find suitable memory type");
		return -1;
	}

	void context::create_command_buffers() {
		if (m_graphics_command_buffers.empty()) {
			m_graphics_command_buffers.reserve(m_swapchain->image_count());
			for (u32 i = 0; i != m_swapchain->image_count(); ++i)
				m_graphics_command_buffers.emplace_back(*this, m_device->graphics_command_pool());
		}

		for (auto& buffer : m_graphics_command_buffers) {
			if (buffer.handle() == nullptr) // has a null handle if unallocated
				buffer.deallocate();
			buffer.allocate(true);
		}

		MYL_CORE_INFO("Vulkan command buffers created");
	}
}
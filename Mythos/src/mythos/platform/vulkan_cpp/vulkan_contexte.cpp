#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_platform.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_vertex_array.hpp"

#include <mythos/core/app.hpp>
#include <mythos/math/vec3.hpp>

namespace myl::vulkane {
	context::context() {
		create_instance();
#ifdef MYL_ENABLE_VALIDATION_LAYERS
		create_debug_messenger();
#endif
		m_surface = platform_create_surface(m_instance);
		MYL_CORE_INFO("Created Vulkan surface");
		
		m_device = std::make_unique<vulkane::device>(*this);

		create_buffers();
	}

	context::~context() { // Must destory in opposite order of creation
		m_vertex_buffer.reset();
		m_index_buffer.reset();

		destroy_command_buffers();

		m_device.reset();

		if (m_surface) {
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
			MYL_CORE_INFO("Destroyed Vulkan surface");
		}
#ifdef MYL_ENABLE_VALIDATION_LAYERS
		destroy_debug_messenger();
#endif
		destroy_instance();
	}

	u32 context::find_memory_index(u32 a_type_filter, VkMemoryPropertyFlags a_property_flags) {
		VkPhysicalDeviceMemoryProperties properties{};
		vkGetPhysicalDeviceMemoryProperties(m_device->physical(), &properties);

		for (u32 i = 0; i != properties.memoryTypeCount; ++i)
			// Check each memory type to see if its bit is set
			if ((a_type_filter & (1 << i)) && (properties.memoryTypes[i].propertyFlags & a_property_flags) == a_property_flags)
				return i;
		/// MYTodo: SHould probs throw here instead of below
		MYL_CORE_WARN("Unable to find suitable memory type");
		return -1; /// MYTodo: Need a better way to have a invalid number, macro maybe?
	}

	static MYL_NO_DISCARD std::vector<const char*> required_validation_layers() {
		std::vector<const char*> required_layers;
#ifdef MYL_ENABLE_VALIDATION_LAYERS
		MYL_CORE_DEBUG("Validation layers enabled");

		required_layers.push_back("VK_LAYER_KHRONOS_validation");

		u32 available_layer_count = 0;
		MYL_VK_CHECK(vkEnumerateInstanceLayerProperties, &available_layer_count, nullptr);
		std::vector<VkLayerProperties> available_layers(available_layer_count);
		MYL_VK_CHECK(vkEnumerateInstanceLayerProperties, &available_layer_count, available_layers.data());

		MYL_CORE_DEBUG("{} available validation layers:", available_layer_count);
		for (auto& layer : available_layers)
			MYL_CORE_DEBUG("\t- {}", layer.layerName);

		for (auto name : required_layers) { // Verify all required layers are available
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

	static MYL_NO_DISCARD std::vector<const char*> required_extensions() {
		std::vector<const char*> extensions;
		platform_required_extensions(&extensions);
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME); // Generic surface extension
#ifdef MYL_ENABLE_VALIDATION_LAYERS
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		MYL_CORE_DEBUG("Required extensions:");
		for (auto& name : extensions)
			MYL_CORE_DEBUG("\t- {}", name);
#endif
		return extensions;
	}

	void context::create_instance() {
		auto& app_info = app::get().info();
		VkApplicationInfo vk_app_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = app_info.name.c_str(),
			.applicationVersion = VK_MAKE_VERSION(app_info.major, app_info.minor, app_info.patch),
			.pEngineName = "Mythos Engine",
			.engineVersion = VK_MAKE_VERSION(MYL_VERSION_MAJOR, MYL_VERSION_MINOR, MYL_VERSION_PATCH),
			.apiVersion = VK_API_VERSION_1_3
		};

		auto validation_layers = required_validation_layers();
		auto extensions = required_extensions();

		VkInstanceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &vk_app_info,
			.enabledLayerCount = static_cast<u32>(validation_layers.size()),
			.ppEnabledLayerNames = validation_layers.data(),
			.enabledExtensionCount = static_cast<u32>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data()
		};

		/// @brief  MYTodo: Make debug messenger here and creat_info.createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		/// Should be trhow below
		MYL_VK_CHECK(vkCreateInstance, &create_info, nullptr, &m_instance);
		MYL_CORE_INFO("Created Vulkan instance");
	}

	void context::create_command_buffers(swapchain& a_swapchain) {
		if (m_graphics_command_buffers.empty()) {
			m_graphics_command_buffers.reserve(a_swapchain.images().size());
			for (u32 i = 0; i != a_swapchain.images().size(); ++i)
				m_graphics_command_buffers.emplace_back(*this, m_device->graphics_command_pool());
		}

		for (auto& buffer : m_graphics_command_buffers) {
			if (buffer.handle() == nullptr) // Has a null handle if unallocated
				buffer.deallocate();
			buffer.allocate(true);
		}
	}

	void context::destroy_instance() {
		vkDestroyInstance(m_instance, nullptr);
		MYL_CORE_INFO("Destroyed instance");
	}

	void context::destroy_command_buffers() {
		for (auto& buffer : m_graphics_command_buffers)
			if (buffer.handle() == nullptr)
				buffer.deallocate();
		m_graphics_command_buffers.clear();
		MYL_CORE_INFO("Destroyed command buffers");
	}

	void context::create_buffers() {
		VkMemoryPropertyFlagBits memory_prop_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		/// MYTodo: This should be dynamic later on
		const u64 vertex_buffer_size = sizeof(f32vec3) * 1024 * 1024;
		m_vertex_buffer = std::make_unique<buffer>(*this, vertex_buffer_size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			memory_prop_flags, true);
		m_geometry_vertex_offset = 0;

		/// MYTodo: This should be dynamic later on
		const u64 index_buffer_size = sizeof(u32) * 1024 * 1024;
		m_index_buffer = std::make_unique<buffer>(*this, index_buffer_size,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			memory_prop_flags, true);
		m_geometry_index_offset = 0;
	}

	void context::upload_data_range(VkCommandPool a_pool, VkFence a_fence, VkQueue a_queue, buffer& a_buffer, u64 a_offset, u64 a_size, void* a_data) {
		// Create a host-visible staging buffer to upload to. Mark it as the source of the transfer.
		VkBufferUsageFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		buffer staging(*this, a_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, flags, true);

		// Load the data into the staging buffer.
		staging.load(0, a_size, 0, a_data);

		// Perform the copy from staging to the device local buffer.
		staging.copy_to(a_pool, a_fence, a_queue, a_buffer.handle(), a_offset, a_size);
	}

#ifdef MYL_ENABLE_VALIDATION_LAYERS
	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* user_data) {
		switch (severity) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: MYL_CORE_TRACE("Vulkan - {}", data->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: MYL_CORE_INFO("Vulkan - {}", data->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: MYL_CORE_WARN("Vulkan - {}", data->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: MYL_CORE_ERROR("Vulkan - {}", data->pMessage); break;
			default: MYL_CORE_DEBUG("Vulkan - {}", data->pMessage); break;
		}
		return VK_FALSE;
	}

	void context::create_debug_messenger() {
		VkDebugUtilsMessengerCreateInfoEXT create_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,// | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = debug_callback,
			.pUserData = nullptr  // Optional
		};

		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
		MYL_CORE_ASSERT(func, "Failed to create debug messenger");
		MYL_CORE_ASSERT(func(m_instance, &create_info, nullptr, &m_debug_messenger) == VK_SUCCESS, "Vulkan - Failed to create debug messenger");
		MYL_CORE_INFO("Created debug messenger ");
	}

	void context::destroy_debug_messenger() {
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
		func(m_instance, m_debug_messenger, nullptr);
		MYL_CORE_INFO("Destroyed debug messenger");
	}
#endif
}
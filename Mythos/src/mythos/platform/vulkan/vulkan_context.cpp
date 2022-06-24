#include "vulkan_context.hpp"
#include "vulkan_platform.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_swapchain.hpp"

#include <mythos/core/app.hpp>
#include <mythos/core/log.hpp>
#include <mythos/math/vec3.hpp>

#include <string.h>

namespace myl::vulkan {
	MYL_NO_DISCARD static std::vector<const char*> required_validation_layers() {
		std::vector<const char*> required_layers;
#ifdef MYL_VK_ENABLE_VALIDATION_LAYERS
		required_layers.push_back("VK_LAYER_KHRONOS_validation");

		u32 layer_count = 0;
		MYL_VK_ASSERT(vkEnumerateInstanceLayerProperties, &layer_count, nullptr);
		std::vector<VkLayerProperties> available_layers(layer_count);
		MYL_VK_ASSERT(vkEnumerateInstanceLayerProperties, &layer_count, available_layers.data());

		MYL_CORE_DEBUG("{} available validation layers:", layer_count);
		for (auto& layer : available_layers)
			MYL_CORE_DEBUG("\t- {}", layer.layerName);

		for (auto required : required_layers) {
			bool found = false;
			for (auto& available : available_layers)
				if (strcmp(required, available.layerName) == 0) {
					found = true;
					break;
				}

			if (!found)
				MYL_CORE_FATAL("Required validation layer '{}' not found", required);
		}
#endif
		return required_layers;
	}

	MYL_NO_DISCARD static std::vector<const char*> required_extensions() {
		std::vector<const char*> extensions;
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME); // Generic surface extension
		required_platform_extensions(&extensions);
#ifdef MYL_VK_ENABLE_VALIDATION_LAYERS
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);  // debug utilities

		MYL_CORE_DEBUG("Required extensions:");
		for (auto ex : extensions)
			MYL_CORE_DEBUG("\t- {}", ex);
#endif
		return extensions;
	}

#ifdef MYL_VK_ENABLE_VALIDATION_LAYERS
	VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT types, const VkDebugUtilsMessengerCallbackDataEXT* callback, void* user_data) {
		switch (severity) {
			default: MYL_FALLTHROUGH;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:		MYL_CORE_ERROR(callback->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:	MYL_CORE_WARN(callback->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:		MYL_CORE_INFO(callback->pMessage); break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:	MYL_CORE_TRACE(callback->pMessage); break;
		}
		return VK_FALSE;
	}
#endif
	MYL_NO_DISCARD constexpr std::string VkPhysicalDeviceType_to_string(VkPhysicalDeviceType type) {
		switch (type) {
			case VK_PHYSICAL_DEVICE_TYPE_OTHER: return "Other";
			case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated";
			case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "Discrete";
			case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "Virtual";
			case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU";
			default: return "Unknown";
		}
	}

	context::context()
		: m_cached_framebuffer_extent{ .width = app::get().window()->size().x, .height = app::get().window()->size().y }
		, m_framebuffer_extent{ .width = (m_cached_framebuffer_extent.width == 0) ? 800 : m_cached_framebuffer_extent.width, .height = (m_cached_framebuffer_extent.height == 0) ? 600 : m_cached_framebuffer_extent.height } { /// MYTodo: Don't like this big piece of code
		create_instance();
		create_surface();
		create_device();
		obtain_queues();
		create_command_pool();
		create_buffers();
	}

	context::~context() { // Must destory in opposite order of creation
		destroy_buffers();
		destroy_command_pool();
		destroy_device();
		destroy_surface();
		destroy_instance();
	}

	u32 context::find_memory_index(u32 a_type_filter, u32 a_property_flags) const {
		VkPhysicalDeviceMemoryProperties memory_properties{};
		vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memory_properties);

		for (u32 i = 0; i < memory_properties.memoryTypeCount; ++i)
			// Check each memory type to see if its bit is set
			if (a_type_filter & (1 << i) && (memory_properties.memoryTypes[i].propertyFlags & a_property_flags) == a_property_flags)
				return i;
		MYL_CORE_WARN("Unable to find suitable memory type");
		return ~0;
	}

	swapchain_support_info context::query_swapchain_support(VkPhysicalDevice a_device) const {
		vulkan::swapchain_support_info info{};
		MYL_VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, a_device, m_surface, &info.capabilities);
		
		u32 format_count = 0;
		MYL_VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR, a_device, m_surface, &format_count, VK_NULL_HANDLE);
		if (format_count != 0) {
			info.formats.resize(format_count); // Safe because formats will always be empty
			MYL_VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR, a_device, m_surface, &format_count, info.formats.data());
		}

		u32 present_mode_count = 0;
		MYL_VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR, a_device, m_surface, &present_mode_count, VK_NULL_HANDLE);
		if (present_mode_count != 0) {
			info.present_modes.resize(present_mode_count); // Safe because present_modes will always be empty
			MYL_VK_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR, a_device, m_surface, &present_mode_count, info.present_modes.data());
		}

		return info;
	}

	device_queue_indices context::get_queue_family_indices(VkPhysicalDevice a_device) const {
		u32 queue_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(a_device, &queue_count, VK_NULL_HANDLE);
		std::vector<VkQueueFamilyProperties> queue_families(queue_count);
		vkGetPhysicalDeviceQueueFamilyProperties(a_device, &queue_count, queue_families.data());

		device_queue_indices indices{};
		u32 min_transfer_score = 255;
		for (u32 i = 0; i < queue_families.size(); ++i) {
			u32 current_transfer_score = 0;

			/// MYBug: Some GPU's can have more then one queue share a single index, this code will crashe if thats the case

			if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphics_index = i;
				++current_transfer_score;
			}

			if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				indices.compute_index = i;
				++current_transfer_score;
			}

			if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
				// Take the index if it is the current lowest. This increases the liklihood that it is a dedicated transfer queue
				if (current_transfer_score <= min_transfer_score) {
					min_transfer_score = current_transfer_score;
					indices.transfer_index = i;
				}
			}

			VkBool32 supports_present = VK_FALSE;
			MYL_VK_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR, a_device, i, m_surface, &supports_present);
			if (supports_present)
				indices.present_index = i;
		}

		return indices;
	}

	bool context::device_meets_requirements(VkPhysicalDevice a_device, const device_requirements& a_requirements, const VkPhysicalDeviceProperties& a_properties, const VkPhysicalDeviceFeatures& a_features, device_queue_indices* a_queue_indices) {
		if (a_requirements.discrete_gpu && a_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return false;

		if (a_requirements.sampler_anisotropy && !a_features.samplerAnisotropy)
			return false;

		const device_queue_indices queue_indices = get_queue_family_indices(a_device);
		if ((a_requirements.graphics && queue_indices.graphics_index == ~0) && // Requires index and that index is not valid
			(a_requirements.present && queue_indices.present_index == ~0) &&
			(a_requirements.compute && queue_indices.compute_index == ~0) &&
			(a_requirements.transfer && queue_indices.transfer_index == ~0))
			return false;

		MYL_CORE_INFO("'{}' meets requirements", std::string(a_properties.deviceName));
		MYL_CORE_INFO("        | Graphics | Present | Compute | Transfer");
		MYL_CORE_INFO("Indices | {:<8} | {:<7} | {:<7} | {:<8}", queue_indices.graphics_index, queue_indices.present_index, queue_indices.compute_index, queue_indices.transfer_index);

		vulkan::swapchain_support_info swapchain_support_info = query_swapchain_support(a_device);
		if (swapchain_support_info.formats.empty() || swapchain_support_info.present_modes.empty())
			return false;

		if (!a_requirements.extensions.empty()) {
			u32 available_extension_count = 0;
			MYL_VK_ASSERT(vkEnumerateDeviceExtensionProperties, a_device, nullptr, &available_extension_count, VK_NULL_HANDLE);
			if (available_extension_count == 0) // No available extensions but there are required extensions
				return false;
			else {
				std::vector<VkExtensionProperties> available_extensions(available_extension_count);
				MYL_VK_ASSERT(vkEnumerateDeviceExtensionProperties, a_device, nullptr, &available_extension_count, available_extensions.data());

				for (auto& required : a_requirements.extensions) {
					bool found = false;
					for (auto& available : available_extensions) {
						if (strcmp(required, available.extensionName) == 0) {
							found = true;
							break;
						}
					}

					if (!found) // Can not find required extension
						return false;
				}
			}
		}

		m_swapchain_support_info = swapchain_support_info;
		*a_queue_indices = queue_indices;
		return true;
	}

	void context::select_physical_device() {
		u32 count = 0;
		MYL_VK_ASSERT(vkEnumeratePhysicalDevices, m_instance, &count, VK_NULL_HANDLE);
		if (count == 0)
			throw vulkan_error("No Vulkan supporting devices found");

		std::vector<VkPhysicalDevice> devices(count);
		MYL_VK_ASSERT(vkEnumeratePhysicalDevices, m_instance, &count, devices.data());

		for (auto device : devices) {
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);
			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(device, &features);
			VkPhysicalDeviceMemoryProperties memory;
			vkGetPhysicalDeviceMemoryProperties(device, &memory);

			std::vector<const char*> extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
			device_requirements requirements{ /// MYTodo: These should be driven by engine config
				.graphics = true,
				.present = true,
				.compute = false, /// MYTodo: Enable
				.transfer = true,
				.sampler_anisotropy = true,
				.discrete_gpu = true,
				.extensions = extensions
			};

			device_queue_indices queue_indices{};
			if (device_meets_requirements(device, requirements, properties, features, &queue_indices)) {
				MYL_CORE_INFO("Selected device '{}'", std::string(properties.deviceName));
				MYL_CORE_INFO("\t- GPU Type: {}", VkPhysicalDeviceType_to_string(properties.deviceType));
				MYL_CORE_INFO("\t- GPU driver version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion), VK_VERSION_MINOR(properties.driverVersion), VK_VERSION_PATCH(properties.driverVersion));
				MYL_CORE_INFO("\t- Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));

				for (u32 i = 0; i != memory.memoryHeapCount; ++i) {
					f32 mem_size_gib = static_cast<f32>(memory.memoryHeaps[i].size) / 1024.f / 1024.f / 1024.f; // bytes to gibibytes
					MYL_CORE_INFO((memory.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ? "\t- Heap {}: Local GPU memory: {:.3f} gib" : "\t- Heap {}: Shared system memory: {:.3f} gib", i, mem_size_gib);
				}

				m_physical_device = device;
				m_queue_indices = queue_indices;

				m_properties = properties;
				m_features = features;
				m_memory_properties = memory;
				break;
			}
		}

		if (!m_physical_device)
			throw vulkan_error("No devices meant requirements");
	}

	void context::obtain_queues() {
		// 0 Because it's the first queue being requested, might need to keep track of this later
		vkGetDeviceQueue(m_device, m_queue_indices.graphics_index, 0, &m_graphics_queue);
		vkGetDeviceQueue(m_device, m_queue_indices.transfer_index, 0, &m_transfer_queue);
		vkGetDeviceQueue(m_device, m_queue_indices.present_index, 0, &m_present_queue);
		vkGetDeviceQueue(m_device, m_queue_indices.compute_index, 0, &m_compute_queue);
		MYL_CORE_INFO("Queues obtained");
	}

	void context::detect_depth_format() {
		std::vector<VkFormat> candidates{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
		u32 flags = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;

		for (auto format : candidates) {
			VkFormatProperties properties{};
			vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &properties);

			if (((properties.linearTilingFeatures & flags) == flags) ||
				((properties.optimalTilingFeatures & flags) == flags)) {
				m_depth_format = format;
				return;
			}
		}

		m_depth_format = VK_FORMAT_UNDEFINED;
	}

	void context::upload(VkCommandPool a_pool, VkFence a_fence, VkQueue a_queue, buffer& a_buffer, u64 a_offset, u64 a_size, void* a_data) {
		// Create a host-visible staging buffer to upload to. Mark it as the source of the transfer.
		VkBufferUsageFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		buffer staging(*this, a_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, flags, true);

		// Load the data into the staging buffer.
		staging.load(0, a_size, 0, a_data);

		// Perform the copy from staging to the device local buffer.
		staging.copy_to(a_buffer.handle(), a_pool, a_fence, a_queue, a_offset, a_size);
	}

	void context::create_command_buffers(swapchain& a_swapchain) {
		m_graphics_command_buffers.clear();
		if (m_graphics_command_buffers.empty())
			for (u32 i = 0; i != a_swapchain.images().size(); ++i)
				m_graphics_command_buffers.push_back(command_buffer(*this));

		for (auto& buf : m_graphics_command_buffers)
			buf.allocate(m_graphics_command_pool, true);
	}

	void context::create_buffers() {
		VkMemoryPropertyFlagBits memory_prop_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	
		/// MYTodo: This should be dynamic later on
		const u64 vertex_buffer_size = sizeof(f32vec3) * 1024 * 1024;
		m_vertex_buffer = std::make_unique<buffer>(*this, vertex_buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, memory_prop_flags, true);
		m_geometry_vertex_offset = 0;
	
		/// MYTodo: This should be dynamic later on
		const u64 index_buffer_size = sizeof(u32) * 1024 * 1024;
		m_index_buffer = std::make_unique<buffer>(*this, index_buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT, memory_prop_flags, true);
		m_geometry_index_offset = 0;
	}

	void context::destroy_buffers() {
		m_vertex_buffer.reset();
		m_index_buffer.reset();
	}

	void context::create_instance() {
		auto& app_info = app::get().info();
		VkApplicationInfo vk_app_info{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pNext = VK_NULL_HANDLE,
			.pApplicationName = app_info.name.c_str(),
			.applicationVersion = VK_MAKE_VERSION(app_info.major, app_info.minor, app_info.patch),
			.pEngineName = MYL_ENGINE_NAME,
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

#ifdef MYL_VK_ENABLE_VALIDATION_LAYERS
		VkDebugUtilsMessengerCreateInfoEXT debug_create_info{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT,
			.pfnUserCallback = debug_callback,
			.pUserData = VK_NULL_HANDLE
		};

		create_info.pNext = &debug_create_info;
#endif
		MYL_VK_VERIFY(vkCreateInstance, &create_info, VK_NULL_HANDLE, &m_instance);
		MYL_CORE_INFO("Created Vulkan instance");

#ifdef MYL_VK_ENABLE_VALIDATION_LAYERS
		PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
		MYL_CORE_ASSERT(func, "Failed to create debug messenger"); /// MYTodo: Should not be a core assert
		MYL_VK_ASSERT(func, m_instance, &debug_create_info, VK_NULL_HANDLE, &m_debug_messenger);
		MYL_CORE_INFO("Created Vulkan debugger");
#endif
	}

	void context::create_surface() {
		m_surface = platform_create_surface(m_instance);
		MYL_CORE_INFO("Created Vulkan surface");
	}

	void context::create_device() {
		select_physical_device();

		/// MYTodo: Need not create additional queues for shared indices, 
		/// MYBug: Test for any queue against any queue
		const bool graphics_shares_present = m_queue_indices.graphics_index == m_queue_indices.present_index;
		const bool graphics_shares_transfer = m_queue_indices.graphics_index == m_queue_indices.transfer_index;
		const bool graphics_shares_compute = m_queue_indices.graphics_index == m_queue_indices.compute_index;

		std::vector<u32> indices;
		indices.push_back(m_queue_indices.graphics_index);
		if (!graphics_shares_present)
			indices.push_back(m_queue_indices.present_index);
		if (!graphics_shares_transfer)
			indices.push_back(m_queue_indices.transfer_index);
		/// MYTodo: Bug above happens here, if it shares a queue, don't push back indice
		///if (!graphics_shares_compute)
		///	indices.push_back(m_queue_indices.compute_index);

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos(indices.size());
		for (u32 i = 0; i != indices.size(); ++i) {
			queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_infos[i].queueFamilyIndex = indices[i];
			queue_create_infos[i].queueCount = 1; /// MYBug: Needs to be able to detect how many queues shader an index

			f32 queue_priority = 1.f;
			queue_create_infos[i].pQueuePriorities = &queue_priority;
		}

		std::vector<const char*> extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		u32 available_extension_count = 0;
		MYL_VK_ASSERT(vkEnumerateDeviceExtensionProperties, m_physical_device, nullptr, &available_extension_count, VK_NULL_HANDLE);
		if (available_extension_count != 0) {
			std::vector<VkExtensionProperties> available_extensions(available_extension_count);
			MYL_VK_ASSERT(vkEnumerateDeviceExtensionProperties, m_physical_device, nullptr, &available_extension_count, available_extensions.data());

			for (auto& available : available_extensions)
				if (strcmp(available.extensionName, "VK_KHR_portability_subset") == 0) {
					extensions.push_back("VK_KHR_portability_subset");
					break;
				}
		}

		/// MYTodo: Should be config driven
		VkPhysicalDeviceFeatures features{
			.samplerAnisotropy = VK_TRUE
		};

		VkDeviceCreateInfo create_info{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<u32>(queue_create_infos.size()),
			.pQueueCreateInfos = queue_create_infos.data(),
			.enabledExtensionCount = static_cast<u32>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
			.pEnabledFeatures = &features
		};

		MYL_VK_ASSERT(vkCreateDevice, m_physical_device, &create_info, VK_NULL_HANDLE, &m_device);
		MYL_CORE_INFO("Created logical device");
	}

	void context::create_command_pool() {
		VkCommandPoolCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = m_queue_indices.graphics_index
		};

		MYL_VK_ASSERT(vkCreateCommandPool, m_device, &info, VK_NULL_HANDLE, &m_graphics_command_pool);
		MYL_CORE_INFO("Created graphics command pool");
	}

	void context::destroy_command_pool() {
		for (auto& buffer : m_graphics_command_buffers) // Must destroy buffers before the pool
			if (buffer.handle() != VK_NULL_HANDLE)
				buffer.deallocate(m_graphics_command_pool);
		m_graphics_command_buffers.clear();
		MYL_CORE_INFO("Destroyed command buffers");

		vkDestroyCommandPool(m_device, m_graphics_command_pool, VK_NULL_HANDLE);
		MYL_CORE_INFO("Destroyed graphics command pool");
	}

	void context::destroy_device() {
		vkDestroyDevice(m_device, VK_NULL_HANDLE);
		MYL_CORE_INFO("Destroyed logical device");
	}

	void context::destroy_surface() {
		vkDestroySurfaceKHR(m_instance, m_surface, VK_NULL_HANDLE);
		MYL_CORE_INFO("Destroyed Vulkan surface");
	}

	void context::destroy_instance() {
#ifdef MYL_VK_ENABLE_VALIDATION_LAYERS
		PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
		func(m_instance, m_debug_messenger, VK_NULL_HANDLE);
		MYL_CORE_INFO("Destroyed Vulkan debugger");
#endif
		vkDestroyInstance(m_instance, VK_NULL_HANDLE);
		MYL_CORE_INFO("Destroyed Vulkan instance");
	}
}
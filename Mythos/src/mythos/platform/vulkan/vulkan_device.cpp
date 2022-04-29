#include "vulkan_device.hpp"
#include "vulkan_context.hpp"

#include <mythos/core/assert.hpp>
#include <mythos/core/except.hpp>

#include <string>
#include <string.h>

/// MYTodo: The design of this really sucks : refer to the other video

namespace myl::vulkan {
	device::device(context& a_context) {
		select_physical_device(a_context);
		create_logical_device();
		get_device_queues();
	}

	device::~device() {
		if (m_logical_device)
			vkDestroyDevice(m_logical_device, nullptr);
		// physical devices are not destroyed
	}

	struct device_requirements {
		bool graphics;
		bool present;
		bool compute;
		bool transfer;
		std::vector<const char*> device_extension_names;
		bool sampler_anisotropy;
		bool discrete_gpu;
	};

	struct device_queue_family_info {
		u32 graphics_family_index;
		u32 present_family_index;
		u32 compute_family_index;
		u32 transfer_family_index;
	};

	static void query_swapchain_support(VkPhysicalDevice a_physical_device, VkSurfaceKHR a_surface, swapchain_support_info* a_out_support_info) {
		// surface capabilities
		MYL_CORE_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(a_physical_device, a_surface, &a_out_support_info->capabilites) == VK_SUCCESS);
		// surface formats
		MYL_CORE_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(a_physical_device, a_surface, &a_out_support_info->format_count, nullptr) == VK_SUCCESS);
		if (a_out_support_info->format_count != 0) {
			if (a_out_support_info->formats.size() == 0)
				a_out_support_info->formats.resize(a_out_support_info->format_count);
			MYL_CORE_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(a_physical_device, a_surface, &a_out_support_info->format_count, a_out_support_info->formats.data()) == VK_SUCCESS);
		}

		// present modes
		MYL_CORE_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(a_physical_device, a_surface, &a_out_support_info->present_mode_count, nullptr) == VK_SUCCESS);
		if (a_out_support_info->present_mode_count != 0) {
			if (a_out_support_info->present_modes.size() == 0)
				a_out_support_info->present_modes.resize(a_out_support_info->present_mode_count);
			MYL_CORE_ASSERT(vkGetPhysicalDeviceSurfacePresentModesKHR(a_physical_device, a_surface, &a_out_support_info->present_mode_count, a_out_support_info->present_modes.data()) == VK_SUCCESS);
		}
	}

	static bool physical_device_meets_requirements(
		VkPhysicalDevice a_device,
		VkSurfaceKHR a_surface,
		const VkPhysicalDeviceProperties& a_properties,
		const VkPhysicalDeviceFeatures& a_features,
		const device_requirements& a_requirements,
		device_queue_family_info* a_out_queue_info,
		swapchain_support_info* a_out_swapchain_support) {

		a_out_queue_info->graphics_family_index = -1;
		a_out_queue_info->present_family_index = -1;
		a_out_queue_info->compute_family_index = -1;
		a_out_queue_info->transfer_family_index = -1;

		if (a_requirements.discrete_gpu && a_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			MYL_CORE_FATAL("No devices supporting Vulkan found.");
			return false;
		}

		u32 queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(a_device, &queue_family_count, nullptr);
		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(a_device, &queue_family_count, queue_families.data());

		u8 min_transfer_score = 255;
		for (u32 i = 0; i != queue_family_count; ++i) { // checking what each queue supports
			u8 current_transfer_score = 0;

			if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				a_out_queue_info->graphics_family_index = i;
				++current_transfer_score;
			}

			if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				a_out_queue_info->compute_family_index = i;
				++current_transfer_score;
			}

			if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
				// take the index if it is the current lowest. This increases the chances that it is a dedicated transfer queue
				if (current_transfer_score <= min_transfer_score) {
					min_transfer_score = current_transfer_score;
					a_out_queue_info->transfer_family_index = i;
				}
			}

			VkBool32 supports_present = VK_FALSE;
			MYL_CORE_ASSERT(vkGetPhysicalDeviceSurfaceSupportKHR(a_device, i, a_surface, &supports_present) == VK_SUCCESS);
			if (supports_present)
				a_out_queue_info->present_family_index = i;
		}

		MYL_CORE_INFO("Graphics | Present | Compute | Transfer | Name");
		MYL_CORE_INFO("       {} |       {} |       {} |        {} | {}",
			static_cast<u32>(a_out_queue_info->graphics_family_index != -1),
			static_cast<u32>(a_out_queue_info->present_family_index != -1),
			static_cast<u32>(a_out_queue_info->compute_family_index != -1),
			static_cast<u32>(a_out_queue_info->transfer_family_index != -1),
			std::string(a_properties.deviceName));

		if ((!a_requirements.graphics || (a_requirements.graphics && a_out_queue_info->graphics_family_index != -1)) && // isn't a requirement or it is a requirement and it is and theres a family index
			(!a_requirements.present || (a_requirements.present && a_out_queue_info->present_family_index != -1)) &&
			(!a_requirements.compute || (a_requirements.compute && a_out_queue_info->compute_family_index != -1)) &&
			(!a_requirements.transfer || (a_requirements.transfer && a_out_queue_info->transfer_family_index != -1))) {
			MYL_CORE_INFO("{} meets requirements", std::string(a_properties.deviceName));
			MYL_CORE_TRACE("Graphics family index: {}", a_out_queue_info->graphics_family_index);
			MYL_CORE_TRACE("Present family index: {}", a_out_queue_info->present_family_index);
			MYL_CORE_TRACE("Transfer family index: {}", a_out_queue_info->transfer_family_index);
			MYL_CORE_TRACE("Compute family index: {}", a_out_queue_info->compute_family_index);

			query_swapchain_support(a_device, a_surface, a_out_swapchain_support); /// MYTodo: should probs return a out_swapchain_support

			if (a_out_swapchain_support->format_count < 1 || a_out_swapchain_support->present_mode_count < 1) {
				MYL_CORE_INFO("Required swapchain support not present");
				return false;
			}

			// device extensions
			if (!a_requirements.device_extension_names.empty()) {
				u32 available_extension_count = 0;
				MYL_CORE_ASSERT(vkEnumerateDeviceExtensionProperties(a_device, nullptr, &available_extension_count, nullptr) == VK_SUCCESS);
				if (available_extension_count != 0) {
					std::vector<VkExtensionProperties> available_extensions(available_extension_count);
					MYL_CORE_ASSERT(vkEnumerateDeviceExtensionProperties(a_device, nullptr, &available_extension_count, available_extensions.data()) == VK_SUCCESS);

					u32 required_extension_count = static_cast<u32>(a_requirements.device_extension_names.size());
					for (auto& required : a_requirements.device_extension_names) {
						bool found = false;
						for (auto& available : available_extensions)
							if (strcmp(required, available.extensionName) == 0) {
								found = true;
								break;
							}

						if (!found) {
							MYL_CORE_INFO("Required extension not found: {}", required);
							return false;
						}
					}
				}
			}

			// sampler anisotropy
			if (a_requirements.sampler_anisotropy && !a_features.samplerAnisotropy) {
				MYL_CORE_INFO("Device does not support samplerAnisotropy");
				return false;
			}

			return true;
		}

		return false;
	}

	void device::select_physical_device(context& a_context) {
		u32 count = 0;
		MYL_CORE_ASSERT(vkEnumeratePhysicalDevices(a_context.instance(), &count, nullptr) == VK_SUCCESS);
		MYL_CORE_DEBUG("{} device{} available", count, count > 1 ? "s" : "");
		if (count == 0)
			throw core_runtime_error("No devices with Vulkan support found");

		std::vector<VkPhysicalDevice> physical_devices(count);
		MYL_CORE_ASSERT(vkEnumeratePhysicalDevices(a_context.instance(), &count, physical_devices.data()) == VK_SUCCESS);

		for (auto& device : physical_devices) {
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(device, &properties);

			VkPhysicalDeviceFeatures features{};
			vkGetPhysicalDeviceFeatures(device, &features);

			VkPhysicalDeviceMemoryProperties memory_properties{};
			vkGetPhysicalDeviceMemoryProperties(device, &memory_properties);

			device_requirements requirements{};
			requirements.graphics = true;
			requirements.present = true;
			//requirements.compute = true; // enable this if compute will be required /// MYTodo: set this up the spec says it should be supported if a queue family is exposed
			requirements.transfer = true;
			requirements.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			requirements.sampler_anisotropy = true;
			requirements.discrete_gpu = true; /// MYTodo: It should be able to run on non-dedicated GPUs

			device_queue_family_info queue_info{};
			if (physical_device_meets_requirements(device, a_context.surface(), properties, features, requirements, &queue_info, &m_swapchain_support_info)) {
				MYL_CORE_INFO("Selected device: {}", std::string(properties.deviceName));
				switch (properties.deviceType) {
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						MYL_CORE_INFO("GPU type: Integrated");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						MYL_CORE_INFO("GPU type: Discrete");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						MYL_CORE_INFO("GPU type: Virtual");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						MYL_CORE_INFO("GPU type: CPU");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_OTHER: MYL_FALLTHROUGH;
					default:
						MYL_CORE_INFO("GPU type: Unknown");
						break;
				}

				MYL_CORE_INFO("GPU driver version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion), VK_VERSION_MINOR(properties.driverVersion), VK_VERSION_PATCH(properties.driverVersion));
				MYL_CORE_INFO("Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));

				// memory information
				for (u32 i = 0; i != memory_properties.memoryHeapCount; ++i) {// not a range because memory_properties.memoryHeaps is a c array
					f32 mem_size_gib = static_cast<f32>(memory_properties.memoryHeaps[i].size) / 1024.f / 1024.f / 1024.f; // bytes to GiB
					(memory_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ?
						MYL_CORE_INFO("Heap {} - Local GPU memory: {:.2f} GiB", i, mem_size_gib) :
						MYL_CORE_INFO("Heap {} - Shared system memory: {:.2f} GiB", i, mem_size_gib);
				}

				m_physical_device = device;
				m_graphics_queue_index = queue_info.graphics_family_index;
				m_present_queue_index = queue_info.present_family_index;
				m_transfer_queue_index = queue_info.transfer_family_index;
				//m_compute_queue_index = queue_info.compute_family_index; // enable if needed

				// keeps a copy of properties, features and memory info for later use
				m_properties = properties;
				m_features = features;
				m_memory_properties = memory_properties;
				break; // a device was selected
			}
		}

		m_physical_device ?
			MYL_CORE_INFO("Physical device selected") :
			throw core_runtime_error("No physical devices meet requirements");
	}

	void device::create_logical_device() {
		MYL_CORE_INFO("Creating logical device");
		// do not create additional queues for shared indices
		bool present_shares_graphics_queue = m_graphics_queue_index == m_present_queue_index;
		bool transfer_shares_graphics_queue = m_graphics_queue_index == m_transfer_queue_index;
		u32 index_count = 1;

		if (!present_shares_graphics_queue)
			++index_count;
		if (!transfer_shares_graphics_queue)
			++index_count;

		std::vector<u32> indices(index_count);
		u8 index = 0;
		indices[index++] = m_graphics_queue_index;
		if (!present_shares_graphics_queue)
			indices[index++] = m_present_queue_index;
		if (!transfer_shares_graphics_queue)
			indices[index++] = m_transfer_queue_index;

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos(index_count);
		for (u32 i = 0; i != index_count; ++i) {
			queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_infos[i].queueFamilyIndex = indices[i];
			queue_create_infos[i].pNext = nullptr;
			queue_create_infos[i].flags = 0;

			if (indices[i] == m_graphics_queue_index) {
				queue_create_infos[i].queueCount = 2;
				f32 queue_priority[2]{ 1.f, 1.f };
				queue_create_infos[i].pQueuePriorities = queue_priority;
			}
			else {
				queue_create_infos[i].queueCount = 1;
				f32 queue_priority = 1.f;
				queue_create_infos[i].pQueuePriorities = &queue_priority;
			}
		}

		// request device features
		VkPhysicalDeviceFeatures device_features{}; /// MYTodo: should be config driven
		device_features.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.queueCreateInfoCount = index_count;
		device_create_info.pQueueCreateInfos = queue_create_infos.data();
		device_create_info.pEnabledFeatures = &device_features;
		device_create_info.enabledExtensionCount = 1;
		const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		device_create_info.ppEnabledExtensionNames = &extension_names;

		MYL_CORE_ASSERT(vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_logical_device) == VK_SUCCESS);
		MYL_CORE_INFO("Logical device created");
	}

	void device::get_device_queues() {
		vkGetDeviceQueue(m_logical_device, m_graphics_queue_index, 0, &m_graphics_queue); // 0 because it's the first queue being requested, might need to keep track of this later
		vkGetDeviceQueue(m_logical_device, m_present_queue_index, 0, &m_present_queue); // 0 because it's the first queue being requested, might need to keep track of this later
		vkGetDeviceQueue(m_logical_device, m_transfer_queue_index, 0, &m_transfer_queue); // 0 because it's the first queue being requested, might need to keep track of this later
		MYL_CORE_INFO("Queues obtained");
	}
}
#include "vulkan_device.hpp"
#include "vulkan_context.hpp"
#include "vulkan_common.hpp"

#include <mythos/core/except.hpp>

#include <string>
#include <string.h>

namespace myl::vulkan {
	device::device(context& a_context) 
		: m_context(a_context) {
		select_physical_device();
		create_logical_device();
		get_device_queues();
	}

	device::~device() {
		if (m_logical_device)
			vkDestroyDevice(m_logical_device, nullptr);
		// physical devices are not destroyed
	}

	swapchain_support_info device::query_swapchain_support(VkPhysicalDevice a_device) {
		vulkan::swapchain_support_info info{};
		// surface capabilities
		MYL_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR, a_device, m_context.surface(), &info.capabilites);
		// surface formats
		u32 format_count = 0;
		MYL_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR, a_device, m_context.surface(), &format_count, nullptr);
		if (format_count != 0) {
			if (info.formats.size() == 0)
				info.formats.resize(format_count);
			MYL_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR, a_device, m_context.surface(), &format_count, info.formats.data());
		}

		// present modes
		u32 present_mode_count = 0;
		MYL_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR, a_device, m_context.surface(), &present_mode_count, nullptr);
		if (present_mode_count != 0) {
			if (info.present_modes.size() == 0)
				info.present_modes.resize(present_mode_count);
			MYL_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR, a_device, m_context.surface(), &present_mode_count, info.present_modes.data());
		}

		return info;
	}

	queue_family_indices device::find_queue_family_indices(VkPhysicalDevice a_device) {
		queue_family_indices indices{};

		u32 queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(a_device, &queue_family_count, nullptr);
		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(a_device, &queue_family_count, queue_families.data());

		u8 min_transfer_score = 255;
		for (u32 i = 0; i != queue_family_count; ++i) { // checking what each queue supports
			u8 current_transfer_score = 0;

			if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphics = i;
				++current_transfer_score;
			}

			if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
				indices.compute = i;
				++current_transfer_score;
			}

			if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
				// take the index if it is the current lowest. This increases the chances that it is a dedicated transfer queue
				if (current_transfer_score <= min_transfer_score) {
					min_transfer_score = current_transfer_score;
					indices.transfer = i;
				}
			}

			VkBool32 supports_present = VK_FALSE;
			//MYL_VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR, a_device, i, m_context.surface(), &supports_present);
			if (supports_present)
				indices.present = i;
		}

		return indices;
	}

	VkFormat device::find_supported_format(const std::vector<VkFormat>& a_candidates, VkFormatFeatureFlags a_flags) {
		for (auto& format : a_candidates) {
			VkFormatProperties properties{};
			vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &properties);

			if ((properties.linearTilingFeatures & a_flags) == a_flags)
				return format;
			else if ((properties.optimalTilingFeatures & a_flags) == a_flags)
				return format;
		}

		MYL_CORE_FATAL("Failed to find supported format");
		return VK_FORMAT_UNDEFINED;
	}

	static bool meets_requirements(const device_requirements& a_requirements, const queue_family_indices& a_queue_indices) {
		return
			(!a_requirements.graphics || (a_requirements.graphics && a_queue_indices.graphics != std::numeric_limits<u32>::max())) && // isn't a requirement or it is a requirement and there's a index, therefore meets requirements
			(!a_requirements.present || (a_requirements.present && a_queue_indices.present != std::numeric_limits<u32>::max())) &&
			(!a_requirements.compute || (a_requirements.compute && a_queue_indices.compute != std::numeric_limits<u32>::max())) &&
			(!a_requirements.transfer || (a_requirements.transfer && a_queue_indices.transfer != std::numeric_limits<u32>::max()));
	}

	bool device::physical_device_meets_requirements(VkPhysicalDevice a_device, const VkPhysicalDeviceProperties& a_properties, const VkPhysicalDeviceFeatures& a_features, const device_requirements& a_requirements, queue_family_indices* a_queue_indices) {
		if (a_requirements.discrete_gpu && a_properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
			MYL_CORE_FATAL("No devices supporting Vulkan found.");
			return false;
		}

		*a_queue_indices = find_queue_family_indices(a_device);
		MYL_CORE_INFO("Graphics | Present | Compute | Transfer | Name");
		MYL_CORE_INFO("{:<8} | {:<7} | {:<7} | {:<8} | {}",
			a_queue_indices->graphics != std::numeric_limits<u32>::max(),
			a_queue_indices->present != std::numeric_limits<u32>::max(),
			a_queue_indices->compute != std::numeric_limits<u32>::max(),
			a_queue_indices->transfer != std::numeric_limits<u32>::max(),
			std::string(a_properties.deviceName));

		if (meets_requirements(a_requirements, *a_queue_indices)) {
			MYL_CORE_INFO("{} meets requirements", std::string(a_properties.deviceName));
			MYL_CORE_TRACE("Graphics family index: {}", a_queue_indices->graphics);
			MYL_CORE_TRACE("Present family index: {}", a_queue_indices->present);
			MYL_CORE_TRACE("Transfer family index: {}", a_queue_indices->transfer);
			MYL_CORE_TRACE("Compute family index: {}", a_queue_indices->compute);

			m_swapchain_support_info = query_swapchain_support(a_device);
			if (m_swapchain_support_info.formats.size() < 1 || m_swapchain_support_info.present_modes.size() < 1) {
				MYL_CORE_INFO("Required swapchain support not present");
				return false;
			}

			// device extensions
			if (!a_requirements.device_extension_names.empty()) {
				u32 available_extension_count = 0;
				MYL_VK_CHECK(vkEnumerateDeviceExtensionProperties, a_device, nullptr, &available_extension_count, nullptr);
				if (available_extension_count != 0) {
					std::vector<VkExtensionProperties> available_extensions(available_extension_count);
					MYL_VK_CHECK(vkEnumerateDeviceExtensionProperties, a_device, nullptr, &available_extension_count, available_extensions.data());

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

	void device::select_physical_device() {
		u32 count = 0;
		MYL_VK_CHECK(vkEnumeratePhysicalDevices, m_context.instance(), &count, nullptr);
		MYL_CORE_DEBUG("{} device{} available", count, count > 1 ? "s" : "");
		if (count == 0)
			throw core_runtime_error("No devices with Vulkan support found");

		std::vector<VkPhysicalDevice> physical_devices(count);
		MYL_VK_CHECK(vkEnumeratePhysicalDevices, m_context.instance(), &count, physical_devices.data());

		for (auto& device : physical_devices) {
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(device, &properties);

			VkPhysicalDeviceFeatures features{};
			vkGetPhysicalDeviceFeatures(device, &features);

			VkPhysicalDeviceMemoryProperties memory_properties{};
			vkGetPhysicalDeviceMemoryProperties(device, &memory_properties);

			device_requirements requirements{
				.graphics = true,
				.present = true,
				.compute = false, // enable this if compute will be required /// MYTodo: set this up the spec says it should be supported if a queue family is exposed
				.transfer = true,
				.sampler_anisotropy = true,
				.discrete_gpu = true /// MYTodo: It should be able to run on non-dedicated GPUs
			};
			requirements.device_extension_names.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

			queue_family_indices queue_info{};
			if (physical_device_meets_requirements(device, properties, features, requirements, &queue_info)) {
				MYL_CORE_INFO("Selected device: {}", std::string(properties.deviceName));
				switch (properties.deviceType) {
					case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
						MYL_CORE_INFO("\t- GPU type: Integrated");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
						MYL_CORE_INFO("\t- GPU type: Discrete");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
						MYL_CORE_INFO("\t- GPU type: Virtual");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_CPU:
						MYL_CORE_INFO("\t- GPU type: CPU");
						break;
					case VK_PHYSICAL_DEVICE_TYPE_OTHER: MYL_FALLTHROUGH;
					default:
						MYL_CORE_INFO("\t- GPU type: Unknown");
						break;
				}

				MYL_CORE_INFO("\t- GPU driver version: {}.{}.{}", VK_VERSION_MAJOR(properties.driverVersion), VK_VERSION_MINOR(properties.driverVersion), VK_VERSION_PATCH(properties.driverVersion));
				MYL_CORE_INFO("\t- Vulkan API version: {}.{}.{}", VK_VERSION_MAJOR(properties.apiVersion), VK_VERSION_MINOR(properties.apiVersion), VK_VERSION_PATCH(properties.apiVersion));

				// memory information
				for (u32 i = 0; i != memory_properties.memoryHeapCount; ++i) {// not a range because memory_properties.memoryHeaps is a c array
					f32 mem_size_gib = static_cast<f32>(memory_properties.memoryHeaps[i].size) / 1024.f / 1024.f / 1024.f; // bytes to GiB
					(memory_properties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) ?
						MYL_CORE_INFO("\t- Heap {} - Local GPU memory: {:.2f} GiB", i, mem_size_gib) :
						MYL_CORE_INFO("\t- Heap {} - Shared system memory: {:.2f} GiB", i, mem_size_gib);
				}

				m_physical_device = device;
				m_queue_indices = queue_info;

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
		bool present_shares_graphics_queue = m_queue_indices.graphics == m_queue_indices.present;
		bool transfer_shares_graphics_queue = m_queue_indices.graphics == m_queue_indices.transfer;
		u32 index_count = 1;

		if (!present_shares_graphics_queue)
			++index_count;
		if (!transfer_shares_graphics_queue)
			++index_count;

		std::vector<u32> indices(index_count);
		u8 index = 0;
		indices[index++] = m_queue_indices.graphics;
		if (!present_shares_graphics_queue)
			indices[index++] = m_queue_indices.present;
		if (!transfer_shares_graphics_queue)
			indices[index++] = m_queue_indices.transfer;

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos(index_count);
		for (u32 i = 0; i != index_count; ++i) {
			queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_infos[i].queueFamilyIndex = indices[i];
			queue_create_infos[i].pNext = nullptr;
			queue_create_infos[i].flags = 0;

			if (indices[i] == m_queue_indices.graphics) { /// MYTodo: apperently this causes issues on some computers?
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

		const char* extension_names = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		VkDeviceCreateInfo device_create_info{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = index_count,
			.pQueueCreateInfos = queue_create_infos.data(),
			.enabledExtensionCount = 1,
			.ppEnabledExtensionNames = &extension_names,
			.pEnabledFeatures = &device_features
		};

		MYL_VK_CHECK(vkCreateDevice, m_physical_device, &device_create_info, nullptr, &m_logical_device);
		MYL_CORE_INFO("Logical device created");
	}

	void device::get_device_queues() {
		vkGetDeviceQueue(m_logical_device, m_queue_indices.graphics, 0, &m_graphics_queue); // 0 because it's the first queue being requested, might need to keep track of this later
		vkGetDeviceQueue(m_logical_device, m_queue_indices.present, 0, &m_present_queue); // 0 because it's the first queue being requested, might need to keep track of this later
		vkGetDeviceQueue(m_logical_device, m_queue_indices.transfer, 0, &m_transfer_queue); // 0 because it's the first queue being requested, might need to keep track of this later
		MYL_CORE_INFO("Queues obtained");
	}
}
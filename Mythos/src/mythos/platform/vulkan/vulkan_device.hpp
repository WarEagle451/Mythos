#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

#include <vector>
#include <limits>

namespace myl::vulkan {
	class context; // fwd declaration

	struct swapchain_support_info {
		VkSurfaceCapabilitiesKHR capabilites{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	//@brief all indices are invalid by default
	struct queue_family_indices {
		u32 graphics = std::numeric_limits<u32>::max();
		u32 present = std::numeric_limits<u32>::max();
		u32 compute = std::numeric_limits<u32>::max(); /// MYTodo: implement
		u32 transfer = std::numeric_limits<u32>::max();
	};

	struct device_requirements {
		bool graphics;
		bool present;
		bool compute;
		bool transfer;
		bool sampler_anisotropy;
		bool discrete_gpu;
		std::vector<const char*> extensions;
	};

	class device {
		context& m_context; // context must outlive device

		VkPhysicalDevice m_physical_device;
		VkDevice m_logical_device;
		swapchain_support_info m_swapchain_support_info;

		queue_family_indices m_queue_indices;
		VkQueue m_graphics_queue;
		VkQueue m_present_queue;
		VkQueue m_transfer_queue;

		VkCommandPool m_graphics_command_pool;

		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;
		VkPhysicalDeviceMemoryProperties m_memory_properties;
	public:
		device(context&);
		~device();

		device(const device&) = delete;
		device& operator=(const device&) = delete;

		MYL_NO_DISCARD VkDevice logical() { return m_logical_device; }
		MYL_NO_DISCARD VkPhysicalDevice physical() { return m_physical_device; }

		MYL_NO_DISCARD const swapchain_support_info& swapchain_support_info() const { return m_swapchain_support_info; }
		const queue_family_indices& queue_indices() const { return m_queue_indices; }
		VkCommandPool graphics_command_pool() { return m_graphics_command_pool; }

		const vulkan::swapchain_support_info& query_swapchain_support(VkPhysicalDevice);
		MYL_NO_DISCARD VkFormat find_supported_format(const std::vector<VkFormat>&, VkFormatFeatureFlags);
	private:
		MYL_NO_DISCARD queue_family_indices find_queue_family_indices(VkPhysicalDevice);
		MYL_NO_DISCARD bool device_meets_requirements(VkPhysicalDevice, const VkPhysicalDeviceProperties&, const VkPhysicalDeviceFeatures&, const device_requirements&, queue_family_indices*);

		void select_physical_device();
		void create_logical_device();
		void get_device_queues();
		void create_command_pool();
	};
}
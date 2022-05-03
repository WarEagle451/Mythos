#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

#include <vector>
#include <limits>

namespace myl::vulkan {
	class context; // fwd declaration

	struct swapchain_support_info {
		VkSurfaceCapabilitiesKHR capabilites;
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

	struct device_requirements { /// MYTodo: probs should stay in the cpp file
		bool graphics;
		bool present;
		bool compute;
		bool transfer;
		std::vector<const char*> device_extension_names;
		bool sampler_anisotropy;
		bool discrete_gpu;
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

		VkFormat m_depth_format; /// MYTodo: Why does this get init in swapchain. Sounds like it should be in swapchain
	public:
		device(context&);
		~device();

		device(const device&) = delete;
		device& operator=(const device&) = delete;

		swapchain_support_info query_swapchain_support(VkPhysicalDevice);
		queue_family_indices find_queue_family_indices(VkPhysicalDevice);
		VkFormat find_supported_format(const std::vector<VkFormat>&, VkFormatFeatureFlags);

		VkDevice& logical() { return m_logical_device; }
		VkPhysicalDevice& physical() { return m_physical_device; }
		swapchain_support_info& swapchain_support_info() { return m_swapchain_support_info; }
		queue_family_indices& queue_indices() { return m_queue_indices; }
		VkFormat& depth_format() { return m_depth_format; }
		VkCommandPool& graphics_command_pool() { return m_graphics_command_pool; }
	private:
		bool physical_device_meets_requirements(VkPhysicalDevice, const VkPhysicalDeviceProperties&, const VkPhysicalDeviceFeatures&, const device_requirements&, queue_family_indices*);

		void select_physical_device();
		void create_logical_device();
		void get_device_queues();
		void create_command_pool();
	};
}
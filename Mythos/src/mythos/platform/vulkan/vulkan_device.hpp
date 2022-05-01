#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

#include <vector> ///MYTemp:

namespace myl::vulkan {
	class context; // fwd declaration

	struct swapchain_support_info {
		VkSurfaceCapabilitiesKHR capabilites;
		u32 format_count;
		std::vector<VkSurfaceFormatKHR> formats;
		u32 present_mode_count;
		std::vector<VkPresentModeKHR> present_modes;
	};

	class device {
		VkPhysicalDevice m_physical_device;
		VkDevice m_logical_device;
		swapchain_support_info m_swapchain_support_info;

		i32 m_graphics_queue_index;
		i32 m_present_queue_index;
		i32 m_transfer_queue_index;

		VkQueue m_graphics_queue;
		VkQueue m_present_queue;
		VkQueue m_transfer_queue;

		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;
		VkPhysicalDeviceMemoryProperties m_memory_properties;

		VkFormat m_depth_format;
	public:
		device(context&);
		~device();

		device(const device&) = delete;
		device& operator=(const device&) = delete;

		VkPhysicalDevice& physical_device() { return m_physical_device; }
		VkDevice& logical_device() { return m_logical_device; }
		swapchain_support_info& swapchain_support() { return m_swapchain_support_info; }

		VkQueue& graphics_queue() { return m_graphics_queue; }
		VkQueue& present_queue() { return m_present_queue; }
		VkQueue& transfer_queue() { return m_transfer_queue; }

		i32& graphics_queue_index() { return m_graphics_queue_index; }
		i32& present_queue_index() { return m_present_queue_index; }
		i32& transfer_queue_index() { return m_transfer_queue_index; }

		VkPhysicalDeviceProperties& properties() { return m_properties; }
		VkPhysicalDeviceFeatures& features() { return m_features; }
		VkPhysicalDeviceMemoryProperties& memory_properties() { return m_memory_properties; }

		VkFormat& depth_format() { return m_depth_format; }

		bool detect_depth_format();
	private:
		void select_physical_device(context&);
		void create_logical_device();
		void get_device_queues();
	};

	void query_swapchain_support(VkPhysicalDevice a_physical_device, VkSurfaceKHR a_surface, swapchain_support_info* a_out_support_info); /// MYTodo: should this be a member function?
}
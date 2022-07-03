#pragma once
#include "vulkan_buffer.hpp"
#include "vulkan_command_buffer.hpp"

#include <memory>
#include <vector>

#ifdef MYL_BUILD_DEBUG
#	define MYL_VK_ENABLE_VALIDATION_LAYERS
#endif

namespace myl::vulkan {
	struct device_requirements {
		bool graphics;
		bool present;
		bool compute;
		bool transfer;

		bool sampler_anisotropy;
		bool discrete_gpu;

		std::vector<const char*> extensions;
	};

	//@brief Invaild indices are set to the upper limit of u32
	struct device_queue_indices {
		u32 graphics_index = ~0u;
		u32 present_index = ~0u;
		u32 compute_index = ~0u;
		u32 transfer_index = ~0u;
	};

	struct swapchain_support_info {
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	class swapchain; // fwd declaration

	class context {
		VkInstance m_instance;

#ifdef MYL_VK_ENABLE_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
		VkSurfaceKHR m_surface;

		// Device members
		VkPhysicalDevice m_physical_device;
		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;
		VkPhysicalDeviceMemoryProperties m_memory_properties;
		swapchain_support_info m_swapchain_support_info;
		VkDevice m_device;
		device_queue_indices m_queue_indices;
		VkQueue m_graphics_queue;
		VkQueue m_transfer_queue;
		VkQueue m_present_queue;
		VkQueue m_compute_queue;

		VkCommandPool m_graphics_command_pool;
		std::vector<command_buffer> m_graphics_command_buffers;
		VkFormat m_depth_format; /// MYTodo: Should this be in swapchain?

		std::unique_ptr<buffer> m_vertex_buffer; /// MYTodo: Should not be here
		std::unique_ptr<buffer> m_index_buffer;
		u64 m_geometry_vertex_offset;
		u64 m_geometry_index_offset;

		u32 m_image_index;
	public:
		context();
		~context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		const VkDevice& device() const { return m_device; }
		u32& image_index() { return m_image_index; } /// MYTodo: Should not be a reference
		VkQueue& graphics_queue() { return m_graphics_queue; }
		VkQueue& transfer_queue() { return m_transfer_queue; }
		VkQueue& present_queue() { return m_present_queue; }
		VkQueue& compute_queue() { return m_compute_queue; }
		VkCommandPool& graphics_cmd_pool() { return m_graphics_command_pool; }
		std::unique_ptr<buffer>& vertex_buffer() { return m_vertex_buffer; }
		std::unique_ptr<buffer>& index_buffer() { return m_index_buffer; }
		std::vector<command_buffer>& graphics_command_buffers() { return m_graphics_command_buffers; }
		VkSurfaceKHR& surface() { return m_surface; }
		swapchain_support_info& swapchain_support_info() { return m_swapchain_support_info; }
		device_queue_indices& queue_indices() { return m_queue_indices; }
		VkFormat& depth_format() { return m_depth_format; } /// MYTodo: Should not be a reference

		u32 find_memory_index(u32 a_type_filter, u32 a_property_flags) const;

		//@return Swapchain support info for a physical device
		vulkan::swapchain_support_info query_swapchain_support(VkPhysicalDevice) const;
		//@brief Updates context's swapchain support info
		void query_swapchain_support() { m_swapchain_support_info = query_swapchain_support(m_physical_device); }

		void detect_depth_format(); ///MYTodo: Should this be exposed?

		void upload(VkCommandPool, VkFence, VkQueue, buffer&, u64 a_offset, u64 a_size, void* a_data); /// MYTemp: Remove when vertex arrays are added

		void create_command_buffers(swapchain&);
	private:
		device_queue_indices get_queue_family_indices(VkPhysicalDevice) const;
		bool device_meets_requirements(VkPhysicalDevice, const device_requirements&, const VkPhysicalDeviceProperties&, const VkPhysicalDeviceFeatures&, device_queue_indices*);
		void select_physical_device();
		void obtain_queues();

		void create_instance();
		void create_surface();
		void create_device();
		void create_command_pool();
		void create_buffers(); /// MYTodo: This should probs be done by the renderer, this also needs to be controled and resized sometimes

		void destroy_buffers();
		void destroy_command_pool();
		void destroy_device();
		void destroy_surface();
		void destroy_instance();
	};
}
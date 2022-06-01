#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

#include <vector>

#ifdef MYL_BUILD_DEBUG
#	define MYL_VK_ENABLE_VALIDATION_LAYERS
#endif

/// MYTodo: Remove, same with below
#include "vulkan_buffer.hpp"
#include <memory>
#include "vulkan_fence.hpp"
#include "vulkan_image.hpp"
#include "vulkan_shader.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_framebuffer.hpp" /// MYTodo: Something is missing an include somewhere

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

	class context {
		VkInstance m_instance;

		/// MYTodo: Compatibilty for kohi system, Redo and remove

		std::unique_ptr<buffer> m_object_vertex_buffer;
		std::unique_ptr<buffer> m_object_index_buffer;

		std::vector<command_buffer> m_graphics_command_buffers;
		std::vector<VkSemaphore> m_image_available_semaphores;
		std::vector<VkSemaphore> m_queue_complete_semaphores;

		std::vector<std::shared_ptr<fence>> m_in_flight_fences;
		std::vector<std::weak_ptr<fence>> m_images_in_flight;

		VkExtent2D m_cached_framebuffer_extent;
		VkExtent2D m_framebuffer_extent;
		u64 m_framebuffer_size_generation; /// MYTemp: What do these do?
		u64 m_framebuffer_size_last_generation; /// MYTemp: What do these do?

		u32 m_image_index;
		bool m_recreating_swapchain = false;

		u64 m_geometry_vertex_offset;
		u64 m_geometry_index_offset;
		///

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
		VkFormat m_depth_format;

		u32 m_current_frame = 0;
	public:
		context();
		~context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		const VkDevice& device() const { return m_device; }
		u32& current_frame() { return m_current_frame; }
		u32& image_index() { return m_image_index; }
		VkQueue& graphics_queue() { return m_graphics_queue; }
		VkQueue& transfer_queue() { return m_transfer_queue; }
		VkQueue& present_queue() { return m_present_queue; }
		VkQueue& compute_queue() { return m_compute_queue; }
		VkCommandPool& graphics_cmd_pool() { return m_graphics_command_pool; }
		std::unique_ptr<buffer>& object_vertex_buffer() { return m_object_vertex_buffer; }
		std::unique_ptr<buffer>& object_index_buffer() { return m_object_index_buffer; }
		std::vector<command_buffer>& graphics_command_buffers() { return m_graphics_command_buffers; }
		std::vector<VkSemaphore>& image_available_semaphores() { return m_image_available_semaphores; }
		std::vector<VkSemaphore>& queue_complete_semaphores() { return m_queue_complete_semaphores; }
		std::vector<std::shared_ptr<fence>>& in_flight_fences() { return m_in_flight_fences; }
		std::vector<std::weak_ptr<fence>>& images_in_flight() { return m_images_in_flight; }
		VkSurfaceKHR& surface() { return m_surface; }
		bool& recreating_swapchain() { return m_recreating_swapchain; }
		swapchain_support_info& swapchain_support_info() { return m_swapchain_support_info; }
		device_queue_indices& queue_indices() { return m_queue_indices; }
		VkFormat& depth_format() { return m_depth_format; }
		VkExtent2D& cached_framebuffer_extent() { return m_cached_framebuffer_extent; }
		VkExtent2D& framebuffer_extent() { return m_framebuffer_extent; }
		u64& framebuffer_size_generation() { return m_framebuffer_size_generation; }
		u64& framebuffer_size_last_generation() { return m_framebuffer_size_last_generation; }
		u64& geometry_vertex_offset() { return m_geometry_vertex_offset; }
		u64& geometry_index_offset() { return m_geometry_index_offset; }

		u32 find_memory_index(u32 a_type_filter, u32 a_property_flags) const;

		//@return Swapchain support info for a physical device
		vulkan::swapchain_support_info query_swapchain_support(VkPhysicalDevice) const;
		//@brief Updates context's swapchain support info
		void query_swapchain_support() { m_swapchain_support_info = query_swapchain_support(m_physical_device); }

		void detect_depth_format(); ///MYTodo: Should this be exposed? 
	private:
		device_queue_indices get_queue_family_indices(VkPhysicalDevice) const;
		bool device_meets_requirements(VkPhysicalDevice, const device_requirements&, const VkPhysicalDeviceProperties&, const VkPhysicalDeviceFeatures&, device_queue_indices*);
		void select_physical_device();
		void obtain_queues();

		void create_instance();
		void create_surface();
		void create_device();
		void create_command_pool();

		void destroy_command_pool();
		void destroy_device();
		void destroy_surface();
		void destroy_instance();
	};
}
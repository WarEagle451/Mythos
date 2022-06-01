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
#include "vulkan_render_pass.hpp"
#include "vulkan_command_buffer.hpp"
#include "vulkan_framebuffer.hpp" /// MYTodo: Something is missing an include somewhere
#include "vulkan_swapchain.hpp"

namespace myl::vulkan4 {
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
	public: /// MYTodo: Temp for easy compatibility
		VkInstance m_instance;

		/// MYTodo: Compatibilty for kohi system, Redo and remove
		u32 cached_framebuffer_width = 0;
		u32 cached_framebuffer_height = 0;
		u32 m_framebuffer_width;
		u32 m_framebuffer_height;
		u64 m_framebuffer_size_generation;
		u64 m_framebuffer_size_last_generation;
		u32 m_current_frame = 0;

		std::unique_ptr<swapchain> swapchain;
		std::unique_ptr<render_pass> main_renderpass;

		std::unique_ptr<vulkan4::buffer> object_vertex_buffer;
		std::unique_ptr<vulkan4::buffer> object_index_buffer;

		std::vector<command_buffer> graphics_command_buffers;

		// darray
		std::vector<VkSemaphore> image_available_semaphores;

		// darray
		std::vector<VkSemaphore> queue_complete_semaphores;

		u32 in_flight_fence_count;
		std::vector<std::shared_ptr<vulkan4::fence>> in_flight_fences;

		// Holds pointers to fences which exist and are owned elsewhere.
		std::vector<std::weak_ptr<vulkan4::fence>> images_in_flight;

		u32 image_index;
		bool recreating_swapchain = false;
		std::unique_ptr<shader> object_shader;

		u64 geometry_vertex_offset;
		u64 geometry_index_offset;
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
	public:
		context();
		~context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		u32 find_memory_index(u32 a_type_filter, u32 a_property_flags) const;

		//@return Swapchain support info for a physical device
		swapchain_support_info query_swapchain_support(VkPhysicalDevice) const;
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
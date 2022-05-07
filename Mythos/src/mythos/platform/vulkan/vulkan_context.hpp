#pragma once
#include "vulkan_device.hpp"
#include "vulkan_command_buffer.hpp"

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

#ifdef MYL_BUILD_DEBUG
#	define MYL_ENABLE_VALIDATION_LAYERS
#endif

namespace myl::vulkan {
	class swapchain; // fwd declaration

	class context {
		VkInstance m_instance;

		VkSurfaceKHR m_surface;

		std::unique_ptr<device> m_device;

		std::vector<command_buffer> m_graphics_command_buffers;

#ifdef MYL_ENABLE_VALIDATION_LAYERS
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
	public:
		context();
		~context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		MYL_NO_DISCARD VkInstance instance() { return m_instance; }
		MYL_NO_DISCARD VkSurfaceKHR surface() { return m_surface; }
		MYL_NO_DISCARD device& device() { return *m_device; }

		MYL_NO_DISCARD i32 find_memory_index(u32 a_type_filter, u32 a_property_flags);

		void create_command_buffers(swapchain&); /// MYTodo: should command buffers be in swapchain
	private:
		void create_instance();

		void destroy_instance();
		void destroy_command_buffers();

#ifdef MYL_ENABLE_VALIDATION_LAYERS
		void create_debug_messenger();
		void destroy_debug_messenger();
#endif
	};
}
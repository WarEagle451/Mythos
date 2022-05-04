#pragma once
#include "vulkan_device.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_render_pass.hpp"
#include "vulkan_command_buffer.hpp"

#include <mythos/defines.hpp>
#include <mythos/core/app.hpp>

#include <memory>
#include <vector>

namespace myl::vulkan {
	class context {
		u32 m_framebuffer_width, m_framebuffer_height;
		VkInstance m_instance;
		VkSurfaceKHR m_surface;
#ifdef MYL_BUILD_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
		std::unique_ptr<device> m_device;
		std::unique_ptr<swapchain> m_swapchain; /// MYTodo: Should this be in backend?
		std::unique_ptr<render_pass> m_main_render_pass;

		std::vector<command_buffer> m_graphics_command_buffers;

		u32 m_image_index; /// MYTodo: what are these for?
		u32 m_current_frame; /// MYTodo: what are these for?
	public:
		context(const app::info&);
		~context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		MYL_NO_DISCARD VkInstance& instance() { return m_instance; }
		MYL_NO_DISCARD VkSurfaceKHR& surface() { return m_surface; }
		MYL_NO_DISCARD device& device() { return *m_device.get(); }
		MYL_NO_DISCARD swapchain& swapchain() { return *m_swapchain.get(); }

		MYL_NO_DISCARD u32 framebuffer_width() const { return m_framebuffer_width; }
		MYL_NO_DISCARD u32 framebuffer_height() const { return m_framebuffer_height; }

		MYL_NO_DISCARD u32& current_frame() { return m_current_frame; }

		MYL_NO_DISCARD i32 find_memory_index(u32 a_type_filter, u32 a_property_flags);
	private:
		void create_command_buffers();
	};
}
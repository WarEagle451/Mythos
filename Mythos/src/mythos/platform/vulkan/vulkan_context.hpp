#pragma once
#include "vulkan_device.hpp"

#include <mythos/defines.hpp>
#include <mythos/core/app.hpp>

#include <memory>

namespace myl::vulkan {
	class context {
		u32 m_framebuffer_width, m_framebuffer_height;
		VkInstance m_instance;
		VkSurfaceKHR m_surface;
#ifdef MYL_BUILD_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
		std::unique_ptr<device> m_device;

		u32 m_image_index; /// MYTodo: what are these for?
		u32 m_current_frame; /// MYTodo: what are these for?
	public:
		context(const app::info&);
		~context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		VkInstance& instance() { return m_instance; }
		VkSurfaceKHR& surface() { return m_surface; }
		device& device() { return *m_device.get(); }

		u32 framebuffer_width() const { return m_framebuffer_width; }
		u32 framebuffer_height() const { return m_framebuffer_height; }

		u32& current_frame() { return m_current_frame; }

		i32 find_memory_index(u32 a_type_filter, u32 a_property_flags);
	};
}
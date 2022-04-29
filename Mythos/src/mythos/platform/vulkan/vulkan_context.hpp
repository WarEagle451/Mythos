#pragma once
#include "vulkan_device.hpp"

#include <mythos/defines.hpp>
#include <mythos/core/app.hpp>

#include <memory>

namespace myl::vulkan {
	class context {
		VkInstance m_instance;
		VkSurfaceKHR m_surface;
#ifdef MYL_BUILD_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
		std::unique_ptr<device> m_device; /// MYTodo: maybe put into backend instead
	public:
		context(const app::info&);
		~context();

		context(const context&) = delete;
		context& operator=(const context&) = delete;

		VkInstance& instance() { return m_instance; }
		VkSurfaceKHR& surface() { return m_surface; }
		device& device() { return *m_device.get(); }

#ifdef MYL_BUILD_DEBUG
		VkDebugUtilsMessengerEXT& debug_messenger() { return m_debug_messenger; };
#endif
	};
}
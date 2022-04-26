#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.hpp>

namespace myl::vulkan {
	class context {
		VkInstance m_instance;
#ifdef MYL_BUILD_DEBUG
		VkDebugUtilsMessengerEXT m_debug_messenger;
#endif
	public:
		VkInstance& instance() { return m_instance; }

#ifdef MYL_BUILD_DEBUG
		VkDebugUtilsMessengerEXT& debug_messenger() { return m_debug_messenger; };
#endif
	};
}
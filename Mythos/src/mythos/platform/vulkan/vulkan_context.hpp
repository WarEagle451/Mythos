#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

namespace myl::vulkan {
	class context {
		VkInstance m_instance;
	public:
		VkInstance& instance() { return m_instance; }
	};
}
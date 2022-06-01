#pragma once
#include <vulkan/vulkan.h>

#include <vector>

namespace myl::vulkan4 {
	void required_platform_extensions(std::vector<const char*>*);

	VkSurfaceKHR platform_create_surface(VkInstance);
}
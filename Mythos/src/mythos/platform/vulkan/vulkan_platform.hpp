#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace myl::vulkan {
	void platform_required_extension_names(std::vector<const char*>*);

	MYL_NO_DISCARD VkSurfaceKHR platform_create_surface(VkInstance& a_instance);
}
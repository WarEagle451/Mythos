#pragma once
#include "vulkan_buffer.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace myl::vulkane { /// MYBug: Redo this or get rid of it
	struct vertex {
		static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
		static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
	};
}
#pragma once
#include "vulkan_buffer.hpp"

#include <vector>

namespace myl::vulkan {
	struct vertex {
		static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
		static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
	};
}
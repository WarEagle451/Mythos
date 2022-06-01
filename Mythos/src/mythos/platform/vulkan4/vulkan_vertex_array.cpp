#include "vulkan_vertex_array.hpp"

#include <mythos/math/vec3.hpp>

namespace myl::vulkan4 {
	std::vector<VkVertexInputBindingDescription> vertex::get_binding_descriptions() { /// MYTemp: This should be configurable, refer the Mythos Legacy's vertex array
		return { // binding, stride, input rate
			{ 0, sizeof(f32vec3), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}

	std::vector<VkVertexInputAttributeDescription> vertex::get_attribute_descriptions() { /// MYTemp: This should be configurable, refer the Mythos Legacy's vertex array
		return { // location, binding, format, offset
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0 },
		};
	}
}
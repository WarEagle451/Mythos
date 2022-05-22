#include "vulkan_vertex_array.hpp"

#include <mythos/core/assert.hpp>

namespace myl::vulkan {
	std::vector<VkVertexInputBindingDescription> vertex_array::vertex::get_binding_descriptions() { /// MYTemp: This should be configurable, refer the Mythos Legacy's vertex array
		return { // binding, stride, input rate
			{ 0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX }
		};
	}

	std::vector<VkVertexInputAttributeDescription> vertex_array::vertex::get_attribute_descriptions() { /// MYTemp: This should be configurable, refer the Mythos Legacy's vertex array
		return { // location, binding, format, offset
			{ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, position) },
			{ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, color) }
		};
	}

	vertex_array::vertex_array(context& a_context, const std::vector<vertex>& a_vertices)
		: m_context(a_context) {
		create_vertex_buffers(a_vertices);
	}

	vertex_array::~vertex_array() {
		vkDestroyBuffer(m_context.device().logical(), m_vertex_buffer, nullptr);
		vkFreeMemory(m_context.device().logical(), m_vertex_buffer_memory, nullptr); /// MYTodo: vulkan has limited mem allocs, fix this
	}

	void vertex_array::bind(VkCommandBuffer a_command_buffer) {
		VkBuffer buffers[]{ m_vertex_buffer };
		VkDeviceSize offsets[]{ 0 };
		vkCmdBindVertexBuffers(a_command_buffer, 0, 1, buffers, offsets);
	}

	void vertex_array::draw(VkCommandBuffer a_command_buffer) {
		vkCmdDraw(a_command_buffer, m_vertex_count, 1, 0, 0);
	}

	void vertex_array::create_vertex_buffers(const std::vector<vertex>& a_vertices) {
		m_vertex_count = static_cast<uint32_t>(a_vertices.size());
		MYL_CORE_ASSERT(m_vertex_count >= 3, "vertex count must be at least 3");

		VkDeviceSize buffer_size = sizeof(a_vertices[0]) * m_vertex_count;
		///m_context.create_buffer(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_vertex_buffer, m_vertex_buffer_memory);

		void* data;
		vkMapMemory(m_context.device().logical(), m_vertex_buffer_memory, 0, buffer_size, 0, &data);
		memcpy(data, a_vertices.data(), static_cast<size_t>(buffer_size));
		vkUnmapMemory(m_context.device().logical(), m_vertex_buffer_memory);
	}
}
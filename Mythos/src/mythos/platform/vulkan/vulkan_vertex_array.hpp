#pragma once
#include "vulkan_context.hpp"

#include <mythos/math/vec3.hpp>

#include <vector>

namespace myl::vulkan {
	class vertex_array {
		context& m_context;
		VkBuffer m_vertex_buffer;
		VkDeviceMemory m_vertex_buffer_memory;
		uint32_t m_vertex_count;
	public:
		struct vertex {
			f32vec3 position;
			f32vec3 color;

			static std::vector<VkVertexInputBindingDescription> get_binding_descriptions();
			static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
		};

		vertex_array(context&, const std::vector<vertex>&);
		~vertex_array();

		vertex_array(const vertex_array&) = delete;
		vertex_array& operator=(const vertex_array&) = delete;

		void bind(VkCommandBuffer);
		void draw(VkCommandBuffer);
	private:
		void create_vertex_buffers(const std::vector<vertex>&);
	};
}
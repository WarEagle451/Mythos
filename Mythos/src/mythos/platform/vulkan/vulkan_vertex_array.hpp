#pragma once
#include "vulkan_context.hpp"

#include <mythos/math/vec3.hpp>

#include <vector>

/// MYTodo: Contains Index buffer and vertex buffer, I put vertex buffer and index buffer into context

namespace myl::vulkan { /// MYBug: Redo this or get rid of it
	class vertex_array {
		context& m_context;
		VkBuffer m_vertex_buffer;
		u32 m_vertex_count;
		VkDeviceMemory m_vertex_buffer_memory;
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
#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

namespace myl::vulkane {
	class context; // fwd declaration

	class image {
		context& m_context; // Context should outlive images

		VkImage m_handle;
		VkDeviceMemory m_memory;
		VkImageView m_view;
		u32 m_width;
		u32 m_height;
	public:
		image(context& a_context, VkImageType a_image_type, u32 a_width, u32 a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageUsageFlags a_usage, VkMemoryPropertyFlags a_memory_flags, bool a_create_view, VkImageAspectFlags a_view_aspect_flags);
		~image();

		VkImageView& view() { return m_view; }
	private:
		void create_view(VkFormat, VkImageAspectFlags);
	};
}
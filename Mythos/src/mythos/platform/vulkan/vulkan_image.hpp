#pragma once
#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

namespace myl::vulkan {
	class context;

	class image {
		context& m_context;

		VkImage m_handle = nullptr;
		VkDeviceMemory m_memory;
		VkImageView m_view;
		u32 m_width;
		u32 m_height;
	public:
		image(context&, VkImageType, u32 a_width, u32 a_height, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, bool a_create_view, VkImageAspectFlags);
		~image();

		VkImageView view() { return m_view; };
	private:
		void create_view(VkFormat, VkImageAspectFlags);
	};
}
#pragma once
#include <vulkan/vulkan.h>

namespace myl::vulkan {
	class context; // fwd declaration

	class image {
		context& m_context;

		VkImage m_handle;
		VkDeviceMemory m_memory;
		VkImageView m_view;
		VkExtent2D m_extent;
	public:
		image(context&, VkImageType, const VkExtent2D&, VkFormat, VkImageTiling, VkImageUsageFlags, VkMemoryPropertyFlags, bool a_create_view, VkImageAspectFlags);
		~image();

		VkImageView view() { return m_view; };
	private:
		void create_view(VkFormat, VkImageAspectFlags);
	};
}
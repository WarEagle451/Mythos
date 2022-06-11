#include "vulkan_image.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"

namespace myl::vulkan {
	image::image(context& a_context, VkImageType a_image_type, const VkExtent2D& a_extent, VkFormat a_format, VkImageTiling a_tiling, VkImageUsageFlags a_usage, VkMemoryPropertyFlags a_memory_flags, bool a_create_view, VkImageAspectFlags a_view_aspect_flags)
		: m_context(a_context)
		, m_extent(a_extent)  {

		VkImageCreateInfo info = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = a_format,
			.extent = { .width = m_extent.width, .height = m_extent.height, .depth = 1 }, /// MYTodo: Support configurable depth.
			.mipLevels = 4, /// MYTodo: Support mip mapping
			.arrayLayers = 1, /// MYTodo: Support number of layers in the image.
			.samples = VK_SAMPLE_COUNT_1_BIT, /// MYTodo: Configurable sample count.
			.tiling = a_tiling,
			.usage = a_usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE, /// MYTodo: Configurable sharing mode.
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		MYL_VK_ASSERT(vkCreateImage, m_context.device(), &info, VK_NULL_HANDLE, &m_handle);

		VkMemoryRequirements memory_requirements{};
		vkGetImageMemoryRequirements(m_context.device(), m_handle, &memory_requirements);

		u32 memory_type = m_context.find_memory_index(memory_requirements.memoryTypeBits, a_memory_flags);
		if (memory_type == ~0)
			MYL_CORE_ERROR("Invaild image, required memory type not found");

		// Allocate memory
		VkMemoryAllocateInfo memory_allocate_info{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memory_requirements.size,
			.memoryTypeIndex = memory_type
		};
		MYL_VK_ASSERT(vkAllocateMemory, m_context.device(), &memory_allocate_info, VK_NULL_HANDLE, &m_memory);

		// Bind the memory
		MYL_VK_ASSERT(vkBindImageMemory, m_context.device(), m_handle, m_memory, 0);/// MYTodo: Configurable memory offset

		if (a_create_view)
			create_view(a_format, a_view_aspect_flags);
	}

	image::~image() {
		if (m_view) vkDestroyImageView(m_context.device(), m_view, VK_NULL_HANDLE);
		if (m_memory) vkFreeMemory(m_context.device(), m_memory, VK_NULL_HANDLE);
		if (m_handle) vkDestroyImage(m_context.device(), m_handle, VK_NULL_HANDLE);
	}

	void image::create_view(VkFormat a_format, VkImageAspectFlags a_aspect_flags) {
		VkImageViewCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = m_handle,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,  /// MYTodo: Make configurable.
			.format = a_format,
			.subresourceRange = { /// MYTodo: Make configurable
				.aspectMask = a_aspect_flags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		MYL_VK_ASSERT(vkCreateImageView, m_context.device(), &info, VK_NULL_HANDLE, &m_view);
	}
}
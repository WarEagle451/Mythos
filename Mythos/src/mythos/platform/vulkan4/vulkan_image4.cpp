#include "vulkan_image.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"

namespace myl::vulkan4 {
	image::image(context& a_context, VkImageType a_image_type, u32 a_width, u32 a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageUsageFlags a_usage, VkMemoryPropertyFlags a_memory_flags, bool a_create_view, VkImageAspectFlags a_view_aspect_flags)
		: m_context(a_context)
		, m_width(a_width)
		, m_height(a_height) {

		VkImageCreateInfo info = {
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = a_format,
			.extent = {
				.width = m_width,
				.height = m_height,
				.depth = 1 /// MYTODO: Support configurable depth.
			},
			.mipLevels = 4, /// MYTODO: Support mip mapping
			.arrayLayers = 1, /// MYTODO: Support number of layers in the image.
			.samples = VK_SAMPLE_COUNT_1_BIT, /// MYTODO: Configurable sample count.
			.tiling = a_tiling,
			.usage = a_usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE, /// MYTODO: Configurable sharing mode.
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		MYL_VK_ASSERT(vkCreateImage, m_context.m_device, &info, VK_NULL_HANDLE, &m_handle);

		// Query memory requirements.
		VkMemoryRequirements memory_requirements;
		vkGetImageMemoryRequirements(m_context.m_device, m_handle, &memory_requirements);

		u32 memory_type = m_context.find_memory_index(memory_requirements.memoryTypeBits, a_memory_flags);
		if (memory_type == ~0) {
			MYL_CORE_ERROR("Required memory type not found. Image not valid.");
		}

		// Allocate memory
		VkMemoryAllocateInfo memory_allocate_info = { VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO };
		memory_allocate_info.allocationSize = memory_requirements.size;
		memory_allocate_info.memoryTypeIndex = memory_type;
		MYL_VK_ASSERT(vkAllocateMemory, m_context.m_device, &memory_allocate_info, VK_NULL_HANDLE, &m_memory);

		// Bind the memory
		MYL_VK_ASSERT(vkBindImageMemory, m_context.m_device, m_handle, m_memory, 0);  /// MyTODO: configurable memory offset.

		create_view(a_format, a_view_aspect_flags);
	}

	image::~image() {
		if (m_view) vkDestroyImageView(m_context.m_device, m_view, VK_NULL_HANDLE);
		if (m_memory) vkFreeMemory(m_context.m_device, m_memory, VK_NULL_HANDLE);
		if (m_handle) vkDestroyImage(m_context.m_device, m_handle, VK_NULL_HANDLE);
	}

	void image::create_view(VkFormat a_format, VkImageAspectFlags a_aspect_flags) {
		VkImageViewCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = m_handle,
			.viewType = VK_IMAGE_VIEW_TYPE_2D,  /// MYTODO: Make configurable.
			.format = a_format,
			.subresourceRange = { ///MYTodo: Make configurable
				.aspectMask = a_aspect_flags,
				.baseMipLevel = 0,
				.levelCount = 1,
				.baseArrayLayer = 0,
				.layerCount = 1
			}
		};

		MYL_VK_ASSERT(vkCreateImageView, m_context.m_device, &info, VK_NULL_HANDLE, &m_view);
	}
}
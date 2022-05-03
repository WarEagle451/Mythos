#include "vulkan_image.hpp"
#include "vulkan_context.hpp"
#include "vulkan_device.hpp"

#include <mythos/core/log.hpp>
#include <mythos/core/assert.hpp>

namespace myl::vulkan {
	image::image(context& a_context, VkImageType a_image_type, u32 a_width, u32 a_height, VkFormat a_format, VkImageTiling a_tiling, VkImageUsageFlags a_usage, VkMemoryPropertyFlags a_memory_flags, bool a_create_view, VkImageAspectFlags a_view_aspect_flags)
		: m_context(a_context)
		, m_width(a_width)
		, m_height(a_height) {
		VkImageCreateInfo image_create_info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = a_format,
			.extent = { .width = m_width, .height = m_height, .depth = 1 }, /// MYTodo: support configable depth
			.mipLevels = 4, /// MYTodo: Support mip mapping
			.arrayLayers = 1, /// MYTodo: Support a number of layers in an image
			.samples = VK_SAMPLE_COUNT_1_BIT, /// MYTodo: Configurable sample count
			.tiling = a_tiling,
			.usage = a_usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE, /// MYTodo: Configurable sharing mode
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED
		};

		MYL_CORE_ASSERT(vkCreateImage(m_context.device().logical(), &image_create_info, nullptr, &m_handle) == VK_SUCCESS);

		// query memory requirements
		VkMemoryRequirements memory_requirements{};
		vkGetImageMemoryRequirements(m_context.device().logical(), m_handle, &memory_requirements);

		i32 memory_type = m_context.find_memory_index(memory_requirements.memoryTypeBits, a_memory_flags);
		if (memory_type == -1)
			MYL_CORE_ERROR("Invaild image, required memory type not found");

		// allocate memory
		VkMemoryAllocateInfo memory_alloc_info{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memory_requirements.size,
			.memoryTypeIndex = static_cast<u32>(memory_type)
		};
		MYL_CORE_ASSERT(vkAllocateMemory(m_context.device().logical(), &memory_alloc_info, nullptr, &m_memory) == VK_SUCCESS); /// MYTodo: maybe replace with something like: m_context.device().allocate_memory(&memory_alloc_info, &m_memory);

		// bind the memory
		MYL_CORE_ASSERT(vkBindImageMemory(m_context.device().logical(), m_handle, m_memory, 0) == VK_SUCCESS); /// MYTodo: configureable memory offset

		// create view
		if (a_create_view) {
			m_view = 0;
			create_view(a_format, a_view_aspect_flags);
		}
	}

	image::~image() {
		if (m_view) vkDestroyImageView(m_context.device().logical(), m_view, nullptr);
		if (m_memory) vkFreeMemory(m_context.device().logical(), m_memory, nullptr);
		if (m_handle) vkDestroyImage(m_context.device().logical(), m_handle, nullptr);
	}

	void image::create_view(VkFormat a_format, VkImageAspectFlags a_aspect_flags) {
		VkImageViewCreateInfo info{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = m_handle,
			.viewType = VK_IMAGE_VIEW_TYPE_2D, /// MYTodo: make configurable
			.format = a_format,
			.subresourceRange = {
				.aspectMask = a_aspect_flags,
				.baseMipLevel = 0, /// MYTodo: make configurable
				.levelCount = 1, /// MYTodo: make configurable
				.baseArrayLayer = 0, /// MYTodo: make configurable
				.layerCount = 1 /// MYTodo: make configurable
			}
		};

		MYL_CORE_ASSERT(vkCreateImageView(m_context.device().logical(), &info, nullptr, &m_view) == VK_SUCCESS);
	}
}
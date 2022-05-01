#include "vulkan_swapchain.hpp"
#include "vulkan_context.hpp"

#include <mythos/core/log.hpp>
#include <mythos/core/assert.hpp>
#include <mythos/algorithm.hpp>

namespace myl::vulkan {
	void swapchain::recreate(u32 a_width, u32 a_height, swapchain* a_swapchain) { /// MYTodo: do better
		destory(a_swapchain);
		create(a_width, a_height, a_swapchain);
	}

	swapchain::swapchain(context& a_context, u32 a_width, u32 a_height) :
		m_context(a_context) {
		create(a_width, a_height, this);
	}

	swapchain::~swapchain() {
		destory(this);
	}

	bool swapchain::acquire_next_image_index(u64 a_nanoseconds_timeout, VkSemaphore a_image_available_semaphore, VkFence a_fence, u32* a_out_image_index) {
		VkResult result = vkAcquireNextImageKHR(m_context.device().logical_device(), m_handle, a_nanoseconds_timeout, a_image_available_semaphore, a_fence, a_out_image_index);
		
		if (result == VK_ERROR_OUT_OF_DATE_KHR) { // trigger swapchain creation then exit render loop
			recreate(m_context.framebuffer_width(), m_context.framebuffer_height(), this);
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			MYL_CORE_FATAL("Failed to acquire swapchain image");
			return false;
		}

		return true;
	}

	void swapchain::present(VkQueue a_graphics_queue, VkQueue a_present_queue, VkSemaphore a_render_complete_semaphore, u32 a_present_image_index) {
		// return the image to the swapchain for presentaion
		VkPresentInfoKHR present_info{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &a_render_complete_semaphore,
			.swapchainCount = 1,
			.pSwapchains = &m_handle,
			.pImageIndices = &a_present_image_index,
			.pResults = nullptr
		};

		VkResult result = vkQueuePresentKHR(a_present_queue, &present_info);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			// swapchain is out of date, suboptimal or a framebuffer resize has occurred. recreate swapchain
			recreate(m_context.framebuffer_width(), m_context.framebuffer_height(), this);
		else if (result != VK_SUCCESS)
			MYL_CORE_FATAL("Failed to present swapchain image");
	}

	void swapchain::create(u32 a_width, u32 a_height, swapchain* a_swapchain) {
		VkExtent2D swapchain_extent = { a_width, a_height };
		a_swapchain->m_max_frames_in_flight = 2; // triple buffering

		// choose swap surface format
		bool found = false;
		for (auto& format : a_swapchain->m_context.device().swapchain_support().formats)
			// preferred formats
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && /// MYTodo: Probs want a different color format
				format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				a_swapchain->m_image_format = format;
				found = true;
				break;
			}

		if (!found)
			a_swapchain->m_image_format = a_swapchain->m_context.device().swapchain_support().formats[0];

		VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR; // guaranteed to exist 
		for (auto& mode : a_swapchain->m_context.device().swapchain_support().present_modes)
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				present_mode = mode;
				break;
			}

		// requery swapchain support
		query_swapchain_support(a_swapchain->m_context.device().physical_device(), a_swapchain->m_context.surface(), &a_swapchain->m_context.device().swapchain_support());

		// swapchain extent
		if (a_swapchain->m_context.device().swapchain_support().capabilites.currentExtent.width != UINT32_MAX)
			swapchain_extent = a_swapchain->m_context.device().swapchain_support().capabilites.currentExtent;

		// clamp to the value allowed by the gpu
		VkExtent2D min = a_swapchain->m_context.device().swapchain_support().capabilites.minImageExtent;
		VkExtent2D max = a_swapchain->m_context.device().swapchain_support().capabilites.maxImageExtent;
		swapchain_extent.width = math::clamp(swapchain_extent.width, min.width, max.width);
		swapchain_extent.height = math::clamp(swapchain_extent.height, min.height, max.height);

		u32 image_count = a_swapchain->m_context.device().swapchain_support().capabilites.minImageCount + 1;
		if (a_swapchain->m_context.device().swapchain_support().capabilites.maxImageCount > 0 && image_count > a_swapchain->m_context.device().swapchain_support().capabilites.maxImageCount) // safe guard
			image_count = a_swapchain->m_context.device().swapchain_support().capabilites.maxImageCount;

		// swapchain create info
		VkSwapchainCreateInfoKHR swapchain_create_info{};
		swapchain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_create_info.surface = a_swapchain->m_context.surface();
		swapchain_create_info.minImageCount = image_count;
		swapchain_create_info.imageFormat = a_swapchain->m_image_format.format;
		swapchain_create_info.imageColorSpace = a_swapchain->m_image_format.colorSpace;
		swapchain_create_info.imageExtent = swapchain_extent;
		swapchain_create_info.imageArrayLayers = 1;
		swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // aka color buffer in opengl

		// setup queue family indices
		if (a_swapchain->m_context.device().graphics_queue_index() != a_swapchain->m_context.device().present_queue_index()) {
			u32 queue_family_indices[]{
				static_cast<u32>(a_swapchain->m_context.device().graphics_queue_index()),
				static_cast<u32>(a_swapchain->m_context.device().present_queue_index())
			};

			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchain_create_info.queueFamilyIndexCount = 2;
			swapchain_create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else {
			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchain_create_info.queueFamilyIndexCount = 0;
			swapchain_create_info.pQueueFamilyIndices = nullptr;
		}

		swapchain_create_info.preTransform = a_swapchain->m_context.device().swapchain_support().capabilites.currentTransform;
		swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_create_info.presentMode = present_mode;
		swapchain_create_info.clipped = VK_TRUE;
		swapchain_create_info.oldSwapchain = nullptr; /// MYTodo: should pass this in recreate swapchain

		MYL_CORE_ASSERT(vkCreateSwapchainKHR(a_swapchain->m_context.device().logical_device(), &swapchain_create_info, nullptr, &a_swapchain->m_handle) == VK_SUCCESS);

		a_swapchain->m_context.current_frame() = 0;

		// images
		a_swapchain->m_image_count = 0;
		MYL_CORE_ASSERT(vkGetSwapchainImagesKHR(a_swapchain->m_context.device().logical_device(), a_swapchain->m_handle, &a_swapchain->m_image_count, nullptr) == VK_SUCCESS);
		a_swapchain->m_images.resize(a_swapchain->m_image_count);
		MYL_CORE_ASSERT(vkGetSwapchainImagesKHR(a_swapchain->m_context.device().logical_device(), a_swapchain->m_handle, &a_swapchain->m_image_count, a_swapchain->m_images.data()) == VK_SUCCESS);

		// views
		a_swapchain->m_views.resize(a_swapchain->m_image_count);
		for (u32 i = 0; i != a_swapchain->m_image_count; ++i) {
			VkImageViewCreateInfo view_info{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.pNext = nullptr,
				// .flags
				.image = a_swapchain->m_images[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = a_swapchain->m_image_format.format,
				//.components
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			};

			MYL_CORE_ASSERT(vkCreateImageView(a_swapchain->m_context.device().logical_device(), &view_info, nullptr, &a_swapchain->m_views[i]) == VK_SUCCESS);
		}

		// depth resources
		if (!a_swapchain->m_context.device().detect_depth_format()) {
			a_swapchain->m_context.device().depth_format() = VK_FORMAT_UNDEFINED; /// MYTodo: why is this not done in detect_depth_format?
			MYL_CORE_FATAL("Failed to find a supported depth format");
		}
		
		// create depth image and its view
		a_swapchain->m_depth_attachment = std::make_unique<image>(
			a_swapchain->m_context,
			VK_IMAGE_TYPE_2D,
			swapchain_extent.width,
			swapchain_extent.height,
			a_swapchain->m_context.device().depth_format(),
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			true,
			VK_IMAGE_ASPECT_DEPTH_BIT);

		MYL_CORE_INFO("Swapchain created");
	}

	void swapchain::destory(swapchain* a_swapchain) {
		a_swapchain->m_depth_attachment.reset();

		// only destory the views, not the images, since those are owned by the swapchain and are destroyed when it is
		for (auto& view : a_swapchain->m_views)
			vkDestroyImageView(a_swapchain->m_context.device().logical_device(), view, nullptr);

		vkDestroySwapchainKHR(a_swapchain->m_context.device().logical_device(), a_swapchain->m_handle, nullptr);

		MYL_CORE_INFO("Swapchain destroyed");
	}
}
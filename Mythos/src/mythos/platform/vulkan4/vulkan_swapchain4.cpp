#include "vulkan_swapchain.hpp"
#include "vulkan_context.hpp"
#include "vulkan_utils.hpp"

#include <mythos/algorithm.hpp>

namespace myl::vulkan4 {
	swapchain::swapchain(context& a_context, u32 a_width, u32 a_height)
		: m_context(a_context) {
		create(a_width, a_height);
	}

	swapchain::~swapchain() {
		destroy();
	}

	bool swapchain::acquire_next_image_index(u64 a_timeout_ns, VkSemaphore a_image_available_semaphore, VkFence a_fence, u32* a_image_index) {
		VkResult result = vkAcquireNextImageKHR(m_context.m_device, m_handle, a_timeout_ns, a_image_available_semaphore, a_fence, a_image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			// Trigger swapchain recreation, then boot out of the render loop.
			recreate(m_context.m_framebuffer_width, m_context.m_framebuffer_height);
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			MYL_CORE_FATAL("Failed to acquire swapchain image!");
			return false;
		}

		return true;
	}

	void swapchain::recreate(u32 a_width, u32 a_height) {
		destroy();
		create(a_width, a_height);
	}

	void swapchain::present(VkQueue a_graphics_queue, VkQueue a_present_queue, VkSemaphore a_render_complete_semaphore, u32 a_present_image_index) {
		VkPresentInfoKHR present_info{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &a_render_complete_semaphore,
			.swapchainCount = 1,
			.pSwapchains = &m_handle,
			.pImageIndices = &a_present_image_index,
			.pResults = VK_NULL_HANDLE,
		};

		VkResult result = vkQueuePresentKHR(a_present_queue, &present_info);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			// Swapchain is out of date, suboptimal or a framebuffer resize has occurred. Trigger swapchain recreation.
			recreate(m_context.m_framebuffer_width, m_context.m_framebuffer_height);
		}
		else if (result != VK_SUCCESS) {
			MYL_CORE_FATAL("Failed to present swap chain image!");
		}

		// Increment (and loop) the index.
		m_context.m_current_frame = (m_context.m_current_frame + 1) % m_max_frames_in_flight;
	}

	void swapchain::create(u32 a_width, u32 a_height) {
		VkExtent2D swapchain_extent = { a_width, a_height };

		// Choose a swap surface format.
		bool found = false;
		for (u32 i = 0; i < m_context.m_swapchain_support_info.formats.size(); ++i) {
			VkSurfaceFormatKHR format = m_context.m_swapchain_support_info.formats[i];
			// Preferred formats
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
				format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				m_image_format = format;
				found = true;
				break;
			}
		}

		if (!found) {
			m_image_format = m_context.m_swapchain_support_info.formats[0];
		}

		VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
		for (u32 i = 0; i < m_context.m_swapchain_support_info.present_modes.size(); ++i) {
			VkPresentModeKHR mode = m_context.m_swapchain_support_info.present_modes[i];
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
				present_mode = mode;
				break;
			}
		}

		// Requery swapchain support.
		m_context.query_swapchain_support();

		// Swapchain extent
		if (m_context.m_swapchain_support_info.capabilities.currentExtent.width != UINT32_MAX) {
			swapchain_extent = m_context.m_swapchain_support_info.capabilities.currentExtent;
		}

		// Clamp to the value allowed by the GPU.
		VkExtent2D min = m_context.m_swapchain_support_info.capabilities.minImageExtent;
		VkExtent2D max = m_context.m_swapchain_support_info.capabilities.maxImageExtent;
		swapchain_extent.width = myl::clamp(swapchain_extent.width, min.width, max.width);
		swapchain_extent.height = myl::clamp(swapchain_extent.height, min.height, max.height);

		u32 image_count = m_context.m_swapchain_support_info.capabilities.minImageCount + 1;
		if (m_context.m_swapchain_support_info.capabilities.maxImageCount > 0 && image_count > m_context.m_swapchain_support_info.capabilities.maxImageCount) {
			image_count = m_context.m_swapchain_support_info.capabilities.maxImageCount;
		}

		m_max_frames_in_flight = image_count - 1;

		// Swapchain create info
		VkSwapchainCreateInfoKHR swapchain_create_info = { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
		swapchain_create_info.surface = m_context.m_surface;
		swapchain_create_info.minImageCount = image_count;
		swapchain_create_info.imageFormat = m_image_format.format;
		swapchain_create_info.imageColorSpace = m_image_format.colorSpace;
		swapchain_create_info.imageExtent = swapchain_extent;
		swapchain_create_info.imageArrayLayers = 1;
		swapchain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// Setup the queue family indices
		if (m_context.m_queue_indices.graphics_index != m_context.m_queue_indices.present_index) {
			u32 queueFamilyIndices[] = {
				(u32)m_context.m_queue_indices.graphics_index,
				(u32)m_context.m_queue_indices.present_index };
			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchain_create_info.queueFamilyIndexCount = 2;
			swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchain_create_info.queueFamilyIndexCount = 0;
			swapchain_create_info.pQueueFamilyIndices = 0;
		}

		swapchain_create_info.preTransform = m_context.m_swapchain_support_info.capabilities.currentTransform;
		swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_create_info.presentMode = present_mode;
		swapchain_create_info.clipped = VK_TRUE;
		swapchain_create_info.oldSwapchain = 0;

		MYL_VK_ASSERT(vkCreateSwapchainKHR, m_context.m_device, &swapchain_create_info, VK_NULL_HANDLE, &m_handle);

		// Start with a zero frame index.
		m_context.m_current_frame = 0;

		// Images
		u32 image_count2 = 0;
		m_views.clear();
		m_images.clear();
		MYL_VK_ASSERT(vkGetSwapchainImagesKHR, m_context.m_device, m_handle, &image_count2, 0);
		m_images.resize(image_count2);
		MYL_VK_ASSERT(vkGetSwapchainImagesKHR, m_context.m_device, m_handle, &image_count2, m_images.data());

		// Views
		m_views.resize(m_images.size());
		for (u32 i = 0; i < m_images.size(); ++i) {
			VkImageViewCreateInfo view_info = { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
			view_info.image = m_images[i];
			view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			view_info.format = m_image_format.format;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			view_info.subresourceRange.baseMipLevel = 0;
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.baseArrayLayer = 0;
			view_info.subresourceRange.layerCount = 1;

			MYL_VK_ASSERT(vkCreateImageView, m_context.m_device, &view_info, VK_NULL_HANDLE, &m_views[i]);
		}

		// Depth resources
		m_context.detect_depth_format();
		if (m_context.m_depth_format == VK_FORMAT_UNDEFINED)
			MYL_CORE_FATAL("Failed to find a supported format!");

		// Create depth image and its view.
		m_depth_attachment = std::make_unique<myl::vulkan4::image>(m_context, VK_IMAGE_TYPE_2D,
			swapchain_extent.width,
			swapchain_extent.height,
			m_context.m_depth_format,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			true,
			VK_IMAGE_ASPECT_DEPTH_BIT);
		MYL_CORE_INFO("Swapchain created successfully.");
	}

	void swapchain::destroy() {
		vkDeviceWaitIdle(m_context.m_device);
		m_depth_attachment.reset();

		// Only destroy the views, not the images, since those are owned by the swapchain and are thus
		// destroyed when it is.
		for (u32 i = 0; i < m_images.size(); ++i)
			vkDestroyImageView(m_context.m_device, m_views[i], VK_NULL_HANDLE);

		vkDestroySwapchainKHR(m_context.m_device,  m_handle, VK_NULL_HANDLE);
	}
}
#include "vulkan_swapchain.hpp"
#include "vulkan_context.hpp"

#include <mythos/core/assert.hpp>
#include <mythos/core/except.hpp>
#include <mythos/algorithm.hpp>

#include <limits>

namespace myl::vulkan {
	swapchain::swapchain(context& a_context, u32 a_width, u32 a_height) 
		: m_context(a_context) {
		create(a_width, a_height);
	}

	swapchain::~swapchain() {
		destroy();
	}

	bool swapchain::acquire_next_image(u64 a_nanoseconds_timeout, VkSemaphore a_image_available_semaphore, VkFence a_fence, u32* a_out_image_index) {
		VkResult result = vkAcquireNextImageKHR(m_context.device().logical(), m_handle, a_nanoseconds_timeout, a_image_available_semaphore, a_fence, a_out_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) { // trigger swapchain creation then exit render loop
			recreate(m_context.framebuffer_width(), m_context.framebuffer_height());
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
			recreate(m_context.framebuffer_width(), m_context.framebuffer_height());
		else if (result != VK_SUCCESS)
			MYL_CORE_FATAL("Failed to present swapchain image");
	}

	void swapchain::recreate(u32 a_width, u32 a_height) { /// MYTodo: do it proper where oldSwapchain gets passed
		destroy();
		create(a_width, a_height);
	}

	static VkSurfaceFormatKHR choose_surface_format(std::vector<VkSurfaceFormatKHR>& formats) {
		for (const auto& format : formats)
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) // preferred formats
				return format;
		return formats[0];
	}

	static VkPresentModeKHR choose_present_mode(std::vector<VkPresentModeKHR>& available_modes) {
		for (const auto& mode : available_modes)
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				return mode;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	static VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, u32 width, u32 height) {
		if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
			return capabilities.currentExtent;
		else // clamp to the value allowed by the gpu
			return VkExtent2D{
				.width = math::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
				.height = math::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
			};
	}

	void swapchain::create(u32 a_width, u32 a_height) {
		m_max_frames_in_flight = 2; // triple buffering
		auto& support_info = m_context.device().swapchain_support_info();

		m_image_format = choose_surface_format(support_info.formats);
		VkPresentModeKHR present_mode = choose_present_mode(support_info.present_modes);

		// requery swapchain support
		support_info = m_context.device().query_swapchain_support(m_context.device().physical());

		VkExtent2D swapchain_extent = choose_swap_extent(support_info.capabilites, a_width, a_height);

		u32 image_count = support_info.capabilites.minImageCount + 1;
		if (support_info.capabilites.maxImageCount > 0 && image_count > support_info.capabilites.maxImageCount) // safe guard
			image_count = support_info.capabilites.maxImageCount;

		VkSwapchainCreateInfoKHR swapchain_info{};
		swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchain_info.surface = m_context.surface();
		swapchain_info.minImageCount = image_count;
		swapchain_info.imageFormat = m_image_format.format;
		swapchain_info.imageColorSpace = m_image_format.colorSpace;
		swapchain_info.imageExtent = swapchain_extent;
		swapchain_info.imageArrayLayers = 1;
		swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // aka color buffer in opengl

		// setup queue family indices
		auto& queue_indices = m_context.device().queue_indices();
		if (queue_indices.graphics != queue_indices.present) {
			u32 queue_family_indices[]{
				queue_indices.graphics,
				queue_indices.present
			};

			swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchain_info.queueFamilyIndexCount = 2;
			swapchain_info.pQueueFamilyIndices = queue_family_indices;
		}
		else {
			swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchain_info.queueFamilyIndexCount = 0;
			swapchain_info.pQueueFamilyIndices = nullptr;
		}

		swapchain_info.preTransform = support_info.capabilites.currentTransform;
		swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchain_info.presentMode = present_mode;
		swapchain_info.clipped = VK_TRUE;
		swapchain_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_context.device().logical(), &swapchain_info, nullptr, &m_handle) != VK_SUCCESS)
			throw core_runtime_error("Failed to create swapchain");

		m_context.current_frame() = 0; /// MYTodo: Shouldn't that member be in the swapchain?

		// images
		u32 image_amount = 0;
		MYL_CORE_ASSERT(vkGetSwapchainImagesKHR(m_context.device().logical(), m_handle, &image_amount, nullptr) == VK_SUCCESS);
		m_images.resize(image_amount);
		MYL_CORE_ASSERT(vkGetSwapchainImagesKHR(m_context.device().logical(), m_handle, &image_amount, m_images.data()) == VK_SUCCESS);

		// views
		m_views.resize(image_amount);
		for (u32 i = 0; i != image_amount; ++i) {
			VkImageViewCreateInfo view_info{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = m_images[i],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = m_image_format.format,
				.subresourceRange = {
					.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
					.baseMipLevel = 0,
					.levelCount = 1,
					.baseArrayLayer = 0,
					.layerCount = 1
				}
			};

			if (vkCreateImageView(m_context.device().logical(), &view_info, nullptr, &m_views[i]) != VK_SUCCESS)
				throw core_runtime_error("Failed to create image view");
		}

		// depth resources
		m_context.device().depth_format() = m_context.device().find_supported_format(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		if (m_context.device().depth_format() == VK_FORMAT_UNDEFINED)
			MYL_CORE_FATAL("Failed to find a supported depth format");

		// create depth image and its view
		m_depth_attachment = std::make_unique<image>(m_context, VK_IMAGE_TYPE_2D, swapchain_extent.width, swapchain_extent.height, m_context.device().depth_format(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void swapchain::destroy() {
		m_depth_attachment.reset();

		// only destory the views, not the images, since those are owned by the swapchain and are destroyed when it is
		for (auto& view : m_views) /// MYTodo: how does above make any sense?
			vkDestroyImageView(m_context.device().logical(), view, nullptr);

		vkDestroySwapchainKHR(m_context.device().logical(), m_handle, nullptr);
	}
}
#include "vulkan_context.hpp"
#include "vulkan_swapchain.hpp"
#include "vulkan_utils.hpp"

#include <mythos/algorithm.hpp>

namespace myl::vulkan {
	MYL_NO_DISCARD constexpr VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
		for (const auto& format : formats)
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) // Preferred formats
				return format;
		return formats[0];
	}

	MYL_NO_DISCARD constexpr VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& available_modes) {
		for (const auto& mode : available_modes)
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				return mode;
		return VK_PRESENT_MODE_FIFO_KHR;
	}
 
	swapchain::swapchain(context& a_context, render_pass* a_render_pass, const VkExtent2D& a_extent)
		: m_context(a_context)
		, m_extent{ .width = (a_extent.width == 0) ? 800 : a_extent.width, .height = (a_extent.height == 0) ? 600 : a_extent.height }
		, m_render_pass(a_render_pass) {
		create_swapchain(m_extent);
		create_depth_resources(m_extent);
		create_images();

		// Framebuffers are generated only if a valid render pass is provided,
		// otherwise they must be generated before the swapchain is used
		if (a_render_pass)
			regenerate_framebuffers(m_extent);

		create_sync_objects();
	}

	swapchain::~swapchain() {
		vkDeviceWaitIdle(m_context.device()); // Wait for Vulkan operations to be done

		destroy_sync_objects();
		destroy_framebuffers();
		destroy_image_views();
		destroy_depth_resources();
		destroy_swapchain();
	}

	bool swapchain::recreate(const VkExtent2D& a_extent) {
		if (m_recreating)
			return false; // If already being recreated, do not try again
		if (a_extent.width == 0 || a_extent.height == 0)
			return false; // Can't recreate swapchain when width or height is 0
		if (a_extent.width == m_extent.width && a_extent.height == m_extent.height)
			return false; // Don't recreate if extent is already correct

		m_recreating = true;
		vkDeviceWaitIdle(m_context.device()); // Wait for Vulkan operations to be done

		m_images_in_flight.clear();

		// Requry in case things have changed
		m_context.query_swapchain_support();
		m_context.detect_depth_format();

		destroy_sync_objects();
		destroy_framebuffers();
		destroy_depth_resources();
		destroy_image_views();
		destroy_swapchain();

		m_extent = a_extent;
		m_render_pass->x() = 0;
		m_render_pass->y() = 0;
		m_render_pass->extent() = m_extent;
		
		create_swapchain(m_extent);
		create_images();
		create_depth_resources(m_extent);
		regenerate_framebuffers(m_extent);
		create_sync_objects();

		for (auto& cmd_buf : m_context.graphics_command_buffers())
			cmd_buf.deallocate(m_context.graphics_cmd_pool());
		m_context.create_command_buffers(*this);

		m_recreating = false;

		return true;
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
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			// Swapchain is out of date, suboptimal or a framebuffer resize has occurred. Trigger swapchain recreation.
			recreate(m_extent);
		else if (result != VK_SUCCESS)
			MYL_CORE_FATAL("Failed to present swap chain image!");

		// Increment (and loop) the index.
		m_current_frame = (m_current_frame + 1) % m_max_frames_in_flight;
	}

	bool swapchain::acquire_next_image_index(u64 a_timeout_ns, VkSemaphore a_image_available_semaphore, VkFence a_fence, u32* a_image_index) {
		VkResult result = vkAcquireNextImageKHR(m_context.device(), m_handle, a_timeout_ns, a_image_available_semaphore, a_fence, a_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			// Trigger swapchain recreation, then boot out of the render loop.
			recreate(m_extent);
			return false;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			MYL_CORE_FATAL("Failed to acquire swapchain image!");
			return false;
		}

		return true;
	}

	void swapchain::regenerate_framebuffers(const VkExtent2D& a_extent) {
		m_framebuffers.clear();
		m_framebuffers.reserve(m_images.size()); // If reserve does not happen, when context.swapchain.framebuffers.emplace_back tries to resize, it will crash
		for (u32 i = 0; i != m_images.size(); ++i) { /// MYTodo: Make this dynamic based on current configured attachments
			std::vector<VkImageView> attachments{
				m_views[i],
				m_depth_attachment->view()
			};

			m_framebuffers.emplace_back(m_context, *m_render_pass, a_extent, attachments);
		}
	}

	void swapchain::create_swapchain(const VkExtent2D& a_extent) {
		VkExtent2D swapchain_extent = a_extent;

		// Choose a swap surface format.
		m_image_format = choose_surface_format(m_context.swapchain_support_info().formats);

		VkPresentModeKHR present_mode = choose_present_mode(m_context.swapchain_support_info().present_modes);

		// Requery swapchain support.
		m_context.query_swapchain_support();

		// Swapchain extent
		if (m_context.swapchain_support_info().capabilities.currentExtent.width != UINT32_MAX)
			swapchain_extent = m_context.swapchain_support_info().capabilities.currentExtent;

		// Clamp to the value allowed by the GPU.
		VkExtent2D min = m_context.swapchain_support_info().capabilities.minImageExtent;
		VkExtent2D max = m_context.swapchain_support_info().capabilities.maxImageExtent;
		swapchain_extent.width = myl::clamp(swapchain_extent.width, min.width, max.width);
		swapchain_extent.height = myl::clamp(swapchain_extent.height, min.height, max.height);

		u32 image_count = m_context.swapchain_support_info().capabilities.minImageCount + 1;
		if (m_context.swapchain_support_info().capabilities.maxImageCount > 0 && image_count > m_context.swapchain_support_info().capabilities.maxImageCount)
			image_count = m_context.swapchain_support_info().capabilities.maxImageCount;

		m_max_frames_in_flight = image_count - 1;

		// Swapchain create info
		VkSwapchainCreateInfoKHR swapchain_create_info{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = m_context.surface(),
			.minImageCount = image_count,
			.imageFormat = m_image_format.format,
			.imageColorSpace = m_image_format.colorSpace,
			.imageExtent = swapchain_extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.preTransform = m_context.swapchain_support_info().capabilities.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = present_mode,
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE
		};

		// Setup the queue family indices
		if (m_context.queue_indices().graphics_index != m_context.queue_indices().present_index) {
			u32 queueFamilyIndices[]{
				m_context.queue_indices().graphics_index,
				m_context.queue_indices().present_index
			};
			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchain_create_info.queueFamilyIndexCount = 2;
			swapchain_create_info.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			swapchain_create_info.queueFamilyIndexCount = 0;
			swapchain_create_info.pQueueFamilyIndices = 0;
		}

		MYL_VK_ASSERT(vkCreateSwapchainKHR, m_context.device(), &swapchain_create_info, VK_NULL_HANDLE, &m_handle);

		// Start with a zero frame index.
		m_current_frame = 0;
	}

	void swapchain::create_depth_resources(const VkExtent2D& a_extent) {
		m_context.detect_depth_format(); /// MYTodo: Why is this not part of swapchain?
		if (m_context.depth_format() == VK_FORMAT_UNDEFINED)
			MYL_CORE_FATAL("Failed to find a supported depth format!");

		m_depth_attachment = std::make_unique<image>(m_context, VK_IMAGE_TYPE_2D, a_extent, m_context.depth_format(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void swapchain::create_images() {
		u32 image_count = 0;
		m_views.clear();
		m_images.clear();
		MYL_VK_ASSERT(vkGetSwapchainImagesKHR, m_context.device(), m_handle, &image_count, VK_NULL_HANDLE);
		m_images.resize(image_count);
		MYL_VK_ASSERT(vkGetSwapchainImagesKHR, m_context.device(), m_handle, &image_count, m_images.data());

		m_views.resize(image_count);
		for (u32 i = 0; i != m_images.size(); ++i) {
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

			MYL_VK_ASSERT(vkCreateImageView, m_context.device(), &view_info, VK_NULL_HANDLE, &m_views[i]);
		}
	}

	void swapchain::create_sync_objects() {
		m_image_available_semaphores.resize(m_max_frames_in_flight);
		m_queue_complete_semaphores.resize(m_max_frames_in_flight);
		m_in_flight_fences.resize(m_max_frames_in_flight);

		for (u8 i = 0; i < m_max_frames_in_flight; ++i) {
			VkSemaphoreCreateInfo semaphore_create_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
			vkCreateSemaphore(m_context.device(), &semaphore_create_info, VK_NULL_HANDLE, &m_image_available_semaphores[i]);
			vkCreateSemaphore(m_context.device(), &semaphore_create_info, VK_NULL_HANDLE, &m_queue_complete_semaphores[i]);

			// Create the fence in a signaled state, indicating that the first frame has already been "rendered".
			// This will prevent the application from waiting indefinitely for the first frame to render since it cannot be rendered until a frame is "rendered" before it.
			m_in_flight_fences[i] = std::make_shared<fence>(m_context, true);
		}

		// In flight fences should not yet exist at this point, so clear the list.
		// These are stored in pointers because the initial state should be 0, and will be 0 when not in use.
		// Acutal fences are not owned by this list.
		m_images_in_flight.resize(m_images.size());
	}

	void swapchain::destroy_sync_objects() {
		for (u8 i = 0; i < m_max_frames_in_flight; ++i) {
			if (m_image_available_semaphores[i]) {
				vkDestroySemaphore(m_context.device(), m_image_available_semaphores[i], VK_NULL_HANDLE);
				m_image_available_semaphores[i] = VK_NULL_HANDLE;
			}

			if (m_queue_complete_semaphores[i]) {
				vkDestroySemaphore(m_context.device(), m_queue_complete_semaphores[i], VK_NULL_HANDLE);
				m_queue_complete_semaphores[i] = VK_NULL_HANDLE;
			}
		}
	}

	void swapchain::destroy_framebuffers() {
		m_framebuffers.clear();
	}

	void swapchain::destroy_image_views() { // Only destroy the views, not the images, since those are owned by the swapchain and are thus destroyed when it is.
		for (u32 i = 0; i < m_images.size(); ++i)
			vkDestroyImageView(m_context.device(), m_views[i], VK_NULL_HANDLE);
		m_views.clear();
	}

	void swapchain::destroy_depth_resources() {
		m_depth_attachment.reset();
	}

	void swapchain::destroy_swapchain() {
		vkDestroySwapchainKHR(m_context.device(), m_handle, VK_NULL_HANDLE);
	}
}
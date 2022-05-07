#include "vulkan_swapchain.hpp"
#include "vulkan_context.hpp"
#include "vulkan_common.hpp"

#include <mythos/algorithm.hpp>

namespace myl::vulkan {
	swapchain::swapchain(context& a_context, u32 a_width, u32 a_height)
		: m_context(a_context)
		, m_max_frames_in_flight(2) // triple buffering
		, m_current_frame(0) {
		create_swapchain(a_width, a_height);
		MYL_CORE_INFO("Vulkan Swapchain created");
		create_image_views();
		create_depth_resources();
		create_render_pass();
		MYL_CORE_INFO("Created render pass"); /// MYTodo: Does this have to come before create_depth_resources?
		regenerate_framebuffers();
		create_sync_objects();
		MYL_CORE_INFO("Vulkan sync objects created");
	}

	swapchain::~swapchain() {
		for (u32 i = 0; i != m_max_frames_in_flight; ++i) {
			vkDestroySemaphore(m_context.device().logical(), m_image_available_semaphore[i], nullptr);
			vkDestroySemaphore(m_context.device().logical(), m_queue_complete_semaphore[i], nullptr);
		}

		m_framebuffers.clear();
		m_render_pass.reset();
		m_depth_attachment.reset();
		destroy_image_views();
		vkDestroySwapchainKHR(m_context.device().logical(), m_handle, nullptr);
	}

	void swapchain::recreate(u32 a_width, u32 a_height) {
		vkDeviceWaitIdle(m_context.device().logical());

		m_framebuffers.clear();
		m_render_pass.reset();
		destroy_image_views();
		vkDestroySwapchainKHR(m_context.device().logical(), m_handle, nullptr);

		create_swapchain(a_width, a_height);
		create_image_views();
		create_render_pass();
		regenerate_framebuffers();
	}

	bool swapchain::acquire_next_image(u64 a_nanoseconds_timeout, VkSemaphore a_image_available_semaphore, VkFence a_fence, u32* a_out_image_index) {
		VkResult result = vkAcquireNextImageKHR(m_context.device().logical(), m_handle, a_nanoseconds_timeout, a_image_available_semaphore, a_fence, a_out_image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) { // trigger swapchain creation then exit render loop
			recreate(m_swapchain_extent.width, m_swapchain_extent.height);
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
			recreate(m_swapchain_extent.width, m_swapchain_extent.height);
		else if (result != VK_SUCCESS)
			MYL_CORE_FATAL("Failed to present swapchain image");

		m_current_frame = (m_current_frame + 1) % m_max_frames_in_flight;
	}

	static MYL_NO_DISCARD VkSurfaceFormatKHR choose_surface_format(const std::vector<VkSurfaceFormatKHR>& formats) {
		for (const auto& format : formats)
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) // preferred formats
				return format;
		return formats[0];
	}

	static MYL_NO_DISCARD VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR>& available_modes) {
		for (const auto& mode : available_modes)
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
				return mode;
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	static MYL_NO_DISCARD VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities, u32 width, u32 height) {
		if (capabilities.currentExtent.width != std::numeric_limits<u32>::max())
			return capabilities.currentExtent;
		else // clamp to the value allowed by the gpu
			return VkExtent2D{
				.width = clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
				.height = clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
	}

	void swapchain::create_swapchain(u32 a_width, u32 a_height) {
		auto& swapchain_support = m_context.device().swapchain_support_info();

		m_image_format = choose_surface_format(swapchain_support.formats);
		VkPresentModeKHR present_mode = choose_present_mode(swapchain_support.present_modes);

		/// MYTodo: got rid of requry that was probs there because of recreating the swapchain
		m_swapchain_extent = choose_swap_extent(swapchain_support.capabilites, a_width, a_height);

		u32 image_count = swapchain_support.capabilites.minImageCount + 1;
		if (swapchain_support.capabilites.maxImageCount > 0 && image_count > swapchain_support.capabilites.maxImageCount) // safe guard
			image_count = swapchain_support.capabilites.maxImageCount;

		VkSwapchainCreateInfoKHR create_info{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = m_context.surface(),
			.minImageCount = image_count,
			.imageFormat = m_image_format.format,
			.imageColorSpace = m_image_format.colorSpace,
			.imageExtent = m_swapchain_extent,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, // aka color buffer in opengl
			.preTransform = swapchain_support.capabilites.currentTransform,
			.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode = present_mode,
			.clipped = VK_TRUE,
			.oldSwapchain = VK_NULL_HANDLE,
		};

		// setup queue family indices
		auto& queue_indices = m_context.device().queue_indices();
		if (queue_indices.graphics != queue_indices.present) {
			u32 queue_family_indices[]{
				queue_indices.graphics,
				queue_indices.present
			};

			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else {
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices = nullptr;
		}

		MYL_VK_CHECK(vkCreateSwapchainKHR, m_context.device().logical(), &create_info, nullptr, &m_handle);

		// images
		u32 image_amount = 0;
		MYL_VK_CHECK(vkGetSwapchainImagesKHR, m_context.device().logical(), m_handle, &image_amount, nullptr);
		m_images.resize(image_amount);
		MYL_VK_CHECK(vkGetSwapchainImagesKHR, m_context.device().logical(), m_handle, &image_amount, m_images.data());
	}

	void swapchain::create_image_views() {
		m_views.resize(m_images.size());
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

			MYL_VK_CHECK(vkCreateImageView, m_context.device().logical(), &view_info, nullptr, &m_views[i]);
		}
	}

	void swapchain::create_depth_resources() {
		m_depth_format = m_context.device().find_supported_format(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		if (m_depth_format == VK_FORMAT_UNDEFINED)
			MYL_CORE_FATAL("Failed to find a supported depth format");

		m_depth_attachment = std::make_unique<image>(m_context, VK_IMAGE_TYPE_2D, m_swapchain_extent.width, m_swapchain_extent.height, m_depth_format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void swapchain::create_render_pass() {
		m_render_pass = std::make_unique<render_pass>(*this, m_context, 0.f, 0.f, static_cast<f32>(m_swapchain_extent.width), static_cast<f32>(m_swapchain_extent.height), f32vec4{ .1f, .1f, .1f, 1.f }, 1.f, 0);
	}

	void swapchain::regenerate_framebuffers() {
		m_framebuffers.clear();
		for (int i = 0; i != m_images.size(); ++i) { /// MYTodo: make this dynamic based on current configured attachments
			std::vector<VkImageView> attachments{
				m_views[i], // color attachment
				m_depth_attachment->view()
			};

			m_framebuffers.emplace_back(std::make_unique<framebuffer>(m_context, *m_render_pass, u32vec2{ m_swapchain_extent.width, m_swapchain_extent.height }, attachments));
		}
	}

	void swapchain::create_sync_objects() {
		m_image_available_semaphore.resize(m_max_frames_in_flight);
		m_queue_complete_semaphore.resize(m_max_frames_in_flight);
		for (u32 i = 0; i != m_max_frames_in_flight; ++i) {
			VkSemaphoreCreateInfo create_info{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};

			vkCreateSemaphore(m_context.device().logical(), &create_info, nullptr, &m_image_available_semaphore[i]);
			vkCreateSemaphore(m_context.device().logical(), &create_info, nullptr, &m_queue_complete_semaphore[i]);

			// create a fence in a signaled state, indicating that the first frame has already been rendered
			// this will prevent the app from waiting forever for the first frame to render
			// cannot be rendered until a frame is "rendered" before it
			m_in_flight_fences.emplace_back(std::make_shared<fence>(m_context, true));
		}
	}

	void swapchain::destroy_image_views() {
		// only destory the views, not the images, since those are owned by the swapchain and are destroyed when it is
		for (auto& view : m_views) /// MYTodo: how does above make any sense?
			vkDestroyImageView(m_context.device().logical(), view, nullptr);
	}
}
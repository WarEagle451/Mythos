#pragma once
#include "vulkan_image.hpp"
#include "vulkan_framebuffer.hpp"

#include <vector>
#include <memory>

namespace myl::vulkan {
	class context; // fwd declaration

	class swapchain {
		context& m_context; // context must outlive swapchain

		u32 m_max_frames_in_flight;

		VkSwapchainKHR m_handle;
		VkSurfaceFormatKHR m_image_format;

		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_views;

		std::unique_ptr<image> m_depth_attachment;
		// used for on-screen rendering
		std::vector<std::unique_ptr<framebuffer>> m_framebuffers; /// MYBug: Have to use uniques because for some reason non-pointers cause vulkan errors, maybe the handle is null and it's trying to kill itself
	public:
		swapchain(context&, u32 a_width, u32 a_height);
		~swapchain();

		swapchain(const swapchain&) = delete;
		swapchain& operator=(const swapchain&) = delete;

		MYL_NO_DISCARD const VkSurfaceFormatKHR& image_format() const { return m_image_format; }
		MYL_NO_DISCARD u64 image_count() const { return m_images.size(); }
		MYL_NO_DISCARD std::vector<std::unique_ptr<framebuffer>>& framebuffers() { return m_framebuffers; }
		MYL_NO_DISCARD std::vector<VkImageView>& views() { return m_views; }
		MYL_NO_DISCARD image* depth_attachment() { return m_depth_attachment.get(); }
		MYL_NO_DISCARD u32 max_frames_in_flight() const { return m_max_frames_in_flight; }

		MYL_NO_DISCARD bool acquire_next_image(u64 a_nanoseconds_timeout, VkSemaphore a_image_available_semaphore, VkFence a_fence, u32* a_out_image_index);
		void present(VkQueue a_graphics_queue, VkQueue a_present_queue, VkSemaphore a_render_complete_semaphore, u32 a_present_image_index);
		void recreate(u32 a_width, u32 a_height);
	private:
		void create(u32 a_width, u32 a_height);
		void destroy();
	};
}
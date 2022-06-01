#pragma once
#include "vulkan_image.hpp"
#include "vulkan_framebuffer.hpp"

#include <mythos/defines.hpp>

#include <vulkan/vulkan.h>

#include <vector>
#include <memory>

namespace myl::vulkan4 {
	class context; // fwd declaration

	class swapchain {
		context& m_context;

		u32 m_max_frames_in_flight;

		VkSwapchainKHR m_handle;
		VkSurfaceFormatKHR m_image_format;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_views;
		std::vector<framebuffer> m_framebuffers;
		std::unique_ptr<image> m_depth_attachment;
	public:
		swapchain(context&, u32 a_width, u32 a_height);
		~swapchain();

		const VkSurfaceFormatKHR& image_format() const { return m_image_format; }
		u32 max_frames_in_flight() const { return m_max_frames_in_flight; }
		std::vector<VkImage>& images() { return m_images; }
		std::vector<framebuffer>& framebuffers() { return m_framebuffers; }
		std::vector<VkImageView>& views() { return m_views; }
		image& depth_attachment() { return *m_depth_attachment; }

		bool acquire_next_image_index(u64 a_timeout_ns, VkSemaphore a_image_available_semaphore, VkFence, u32* a_image_index);

		void recreate(u32 a_width, u32 a_height);
		void present(VkQueue a_graphics_queue, VkQueue a_present_queue, VkSemaphore a_render_complete_semaphore, u32 a_present_image_index);
	private:
		void create(u32 a_width, u32 a_height);
		void destroy();
	};
}
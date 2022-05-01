#pragma once
#include "vulkan_image.hpp"

#include <vector>
#include <memory>

namespace myl::vulkan {
	class context; // fwd declaration

	class swapchain {
		context& m_context; // context must outlive swapchain

		VkSurfaceFormatKHR m_image_format;
		u8 m_max_frames_in_flight;
		VkSwapchainKHR m_handle;
		u32 m_image_count;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_views; /// MYTodo: should these be arrays?

		std::unique_ptr<image> m_depth_attachment;
	public:
		static void recreate(u32 a_width, u32 a_height, swapchain*);

		swapchain(context& a_context, u32 a_width, u32 a_height); /// MYTodo: should probs hold a ref to context
		~swapchain();

		swapchain(const swapchain&) = delete;
		swapchain& operator=(const swapchain&) = delete;

		bool acquire_next_image_index(u64 a_nanoseconds_timeout, VkSemaphore a_image_available_semaphore, VkFence a_fence, u32* a_out_image_index);

		void present(VkQueue a_graphics_queue, VkQueue a_present_queue, VkSemaphore a_render_complete_semaphore, u32 a_present_image_index);
	private:
		static void create(u32 a_width, u32 a_height, swapchain* a_swapchain); /// MYTodo: get this all in the constructor
		static void destory(swapchain* a_swapchain); /// MYTodo: get this all in the destructor
	};
}
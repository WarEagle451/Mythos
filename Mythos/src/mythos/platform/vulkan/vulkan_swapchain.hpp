#pragma once
#include "vulkan_fence.hpp"
#include "vulkan_framebuffer.hpp"
#include "vulkan_image.hpp"
#include "vulkan_render_pass.hpp"

#include <memory>
#include <vector>

namespace myl::vulkan {
	class context; // fwd declaration

	class swapchain {
		context& m_context;

		u32 m_max_frames_in_flight;
		VkExtent2D m_extent;

		VkSwapchainKHR m_handle;
		VkSurfaceFormatKHR m_image_format;
		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_views;
		std::vector<framebuffer> m_framebuffers;
		std::unique_ptr<image> m_depth_attachment;

		std::vector<VkSemaphore> m_image_available_semaphores;
		std::vector<VkSemaphore> m_queue_complete_semaphores;
		std::vector<std::shared_ptr<fence>> m_in_flight_fences;
		std::vector<std::weak_ptr<fence>> m_images_in_flight;
		u32 m_current_frame = 0;

		render_pass* m_render_pass;
		bool m_recreating = false;
	public:
		//@param render_pass*: A non-owning pointer to a render pass
		swapchain(context&, render_pass*, const VkExtent2D&);
		~swapchain();

		swapchain(const swapchain&) = delete;
		swapchain& operator=(const swapchain&) = delete;

		const VkSurfaceFormatKHR& image_format() const { return m_image_format; }
		u32 max_frames_in_flight() const { return m_max_frames_in_flight; }
		std::vector<VkImage>& images() { return m_images; }
		std::vector<framebuffer>& framebuffers() { return m_framebuffers; }
		std::vector<VkImageView>& views() { return m_views; }
		image& depth_attachment() { return *m_depth_attachment; }

		std::vector<VkSemaphore>& image_available_semaphores() { return m_image_available_semaphores; }
		std::vector<VkSemaphore>& queue_complete_semaphores() { return m_queue_complete_semaphores; }
		std::vector<std::shared_ptr<fence>>& in_flight_fences() { return m_in_flight_fences; }
		std::vector<std::weak_ptr<fence>>& images_in_flight() { return m_images_in_flight; }
		u32 current_frame() const { return m_current_frame; }
		bool recreating() const { return m_recreating; }
		const VkExtent2D& extent() const { return m_extent; }

		//@brief Sets the render pass when it cannot be set in the constructor
		void set_render_pass(render_pass* a_render_pass) { m_render_pass = a_render_pass; }

		bool recreate(const VkExtent2D&);
		void present(VkQueue a_graphics_queue, VkQueue a_present_queue, VkSemaphore a_render_complete_semaphore, u32 a_present_image_index);

		bool acquire_next_image_index(u64 a_timeout_ns, VkSemaphore a_image_available_semaphore, VkFence, u32* a_image_index);
		void regenerate_framebuffers(const VkExtent2D&);
	private:
		void create_swapchain(const VkExtent2D&);
		void create_depth_resources(const VkExtent2D&);
		void create_images();
		// regenerate_frambuffers
		void create_sync_objects();

		void destroy_sync_objects();
		void destroy_framebuffers();
		void destroy_image_views();
		void destroy_depth_resources();
		void destroy_swapchain();
	};
}
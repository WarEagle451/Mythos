#include "vulkan_image.hpp"
#include "vulkan_render_pass.hpp"
#include "vulkan_framebuffer.hpp"
#include "vulkan_fence.hpp"

#include <vector>
#include <memory>

namespace myl::vulkan {
	class context; // fwd declaration

	class swapchain {
		context& m_context; // context must outlive swapchain

		VkSwapchainKHR m_handle;
		VkSurfaceFormatKHR m_image_format;

		std::vector<VkImage> m_images;
		std::vector<VkImageView> m_views;

		VkFormat m_depth_format;
		std::unique_ptr<image> m_depth_attachment;

		std::unique_ptr<render_pass> m_render_pass;

		std::vector<std::unique_ptr<framebuffer>> m_framebuffers;

		std::vector<VkSemaphore> m_image_available_semaphore;
		std::vector<VkSemaphore> m_queue_complete_semaphore;

		std::vector<std::shared_ptr<fence>> m_in_flight_fences;
		std::vector<std::weak_ptr<fence>> m_images_in_flight;

		VkExtent2D m_swapchain_extent;

		u8 m_max_frames_in_flight;
		u8 m_current_frame;
	public:
		swapchain(context&, u32 a_width, u32 a_height);
		~swapchain();

		swapchain(const swapchain&) = delete;
		swapchain& operator=(const swapchain&) = delete;

		std::vector<VkImage>& images() { return m_images; }
		VkFormat depth_format() const { return m_depth_format; }
		VkSurfaceFormatKHR image_format() const { return m_image_format; }

		void recreate(u32 a_width, u32 a_height);
		bool acquire_next_image(u64 a_nanoseconds_timeout, VkSemaphore a_image_available_semaphore, VkFence a_fence, u32* a_out_image_index);
		void present(VkQueue a_graphics_queue, VkQueue a_present_queue, VkSemaphore a_render_complete_semaphore, u32 a_present_image_index);
	private:
		void create_swapchain(u32 a_width, u32 a_height);
		void create_image_views();
		void create_depth_resources();
		void create_render_pass();
		void regenerate_framebuffers();
		void create_sync_objects();

		void destroy_image_views();
	};
}
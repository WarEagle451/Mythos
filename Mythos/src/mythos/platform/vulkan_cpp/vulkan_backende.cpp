#include "vulkan_backend.hpp"
#include "vulkan_shader.hpp"
#include "vulkan_vertex_array.hpp"

#include <mythos/core/log.hpp>
#include <mythos/math/vec3.hpp>

/// MYTodo: Vulkan contructors should throw if they fail

namespace myl::vulkane {
	backend::backend()
		: m_context()
		, m_swapchain(m_context, 800, 600) { /// MYTodo: Have a configurable size to start
		m_context.create_command_buffers(m_swapchain);
		MYL_CORE_INFO("Created command buffers");

		m_shader = create_shader("resources/shaders/shader.glsl"); /// MYTodo: DO NOT HARD CODE THIS

		/// MYTemp: Test geometry
		const u32 vert_count = 3;
		f32vec3 vertex_arrays[vert_count]{
			{ 0.f, -.5f, 0.f },
			{ .5f, .5f, 0.f },
			{ 0.f, .5f, 0.f }
		};
		
		const u32 index_count = 3;
		u32 indices[index_count]{ 0, 1, 2 };
		
		m_context.upload_data_range(m_context.device().graphics_command_pool(), nullptr, m_context.device().graphics_queue(), m_context.vertex_buffer(), 0, sizeof(f32vec3) * vert_count, &vertex_arrays);
		m_context.upload_data_range(m_context.device().graphics_command_pool(), nullptr, m_context.device().graphics_queue(), m_context.index_buffer(), 0, sizeof(u32) * index_count, &indices);
		/// End of temp code

		MYL_CORE_INFO("Created Vulkan backend");
	}

	backend::~backend() { // C++ standard has members destructors called in opposite order of creation
		vkDeviceWaitIdle(m_context.device().logical()); // Waits for all graphics operations to cease

		MYL_CORE_INFO("Destroying Vulkan backend");
		m_shader.reset(); // Deleting shader object
	}

	bool backend::begin() {
		device& device = m_context.device();

		if (m_swapchain.recreating()) {
			VkResult result = vkDeviceWaitIdle(device.logical());
			if (!result_is_success(result)) {
				MYL_CORE_ERROR("Vulkan backend vkDeviceWaitIdle failed: {}", VkResult_to_string(result, true));
				return false;
			}
			MYL_CORE_INFO("Recreating swapchain, exiting render loop");
			return false;
		}

		/// MYTodo: Kohi 21 31:30; Has swapchain recreating code here. But I don't think it's needed for the way I've done it
		/// MYTodo: Should this be the only place where the swapchain can be recreated?

		// Wait for the execution of the current frame to complete. The fence being free will allow this one to move on
		if (!m_swapchain.in_flight_fences()[m_swapchain.current_frame()]->wait(std::numeric_limits<u64>::max())) {
			MYL_CORE_WARN("In-flight fence wait failure");
			return false;
		}

		// Acquire the next image from the swapchain. Pass along the semaphore that should signal when this completes
		// This same semaphore will later be waited on by the queue submission to ensure this image is available
		u32 image_index = 0;
		if (!m_swapchain.acquire_next_image(std::numeric_limits<u64>::max(), m_swapchain.image_available_semaphores()[m_swapchain.current_frame()], nullptr, &image_index))
			return false;
		m_context.set_image_index(image_index);

		// Begin recording command buffers
		auto& command_buffer = m_context.get_graphics_command_buffer(m_context.image_index());
		command_buffer.reset();

		command_buffer.begin(false, false, false);

		/// MYTodo: This makes it so it renders from the bottom left, like opengl, but I want it to be rendered from the top left BECAUSE IT MAKES MORE SENSE
		const VkExtent2D& swapchain_extent = m_swapchain.swapchain_extent();

		m_swapchain.render_pass().begin(command_buffer, m_swapchain.framebuffers()[m_context.image_index()]->handle());

		VkViewport viewport{ // Dynamic state
			.x = 0.f,
			.y = static_cast<f32>(swapchain_extent.height),
			.width = static_cast<f32>(swapchain_extent.width),
			.height = -static_cast<f32>(swapchain_extent.height),
			.minDepth = 0.f,
			.maxDepth = 1.f
		};
		vkCmdSetViewport(command_buffer.handle(), 0, 1, &viewport);

		VkRect2D scissor{ // Scissor
			.offset{.x = 0, .y = 0 },
			.extent = swapchain_extent
		};
		vkCmdSetScissor(command_buffer.handle(), 0, 1, &scissor);

		/// MYTemp: Test code
		m_shader->bind();

		// Bind vertex buffer at offset
		VkDeviceSize offsets[]{ 0 };
		vkCmdBindVertexBuffers(command_buffer.handle(), 0, 1, &m_context.vertex_buffer().handle(), offsets);
		vkCmdBindIndexBuffer(command_buffer.handle(), m_context.index_buffer().handle(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(command_buffer.handle(), 3, 1, 0, 0, 0);
		/// End of temp code

		return true;
	}

	void backend::end() {
		command_buffer& command_buffer = m_context.get_graphics_command_buffer(m_context.image_index());
		
		m_swapchain.render_pass().end(command_buffer);
		command_buffer.end();

		auto& image_in_flight = m_swapchain.images_in_flight()[m_context.image_index()];
		// Make sure a previous frame is not using this image, eg, its fence is being waited on
		if (!image_in_flight.expired()) // was frame
			image_in_flight.lock()->wait(std::numeric_limits<u64>::max());
		
		// Mark the image fence as in use by this frame
		image_in_flight = m_swapchain.in_flight_fences()[m_swapchain.current_frame()];
		
		// Reset the fence for use on the next frame
		m_swapchain.in_flight_fences()[m_swapchain.current_frame()]->reset();
		
		// Submit the queue and wait for the operation to complete
		// Begin queue submission
		VkSubmitInfo submit_info{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &m_swapchain.image_available_semaphores()[m_swapchain.current_frame()],
			.commandBufferCount = 1,
			.pCommandBuffers = &command_buffer.handle(),
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &m_swapchain.queue_complete_semaphores()[m_swapchain.current_frame()],
		};
		
		// Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio
		// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent color attachment
		// Writes from executing until the semaphore signals (i.e one frame is presented at a time)
		VkPipelineStageFlags flags[1]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_info.pWaitDstStageMask = flags;
		
		VkResult result = vkQueueSubmit(m_context.device().graphics_queue(), 1, &submit_info, m_swapchain.in_flight_fences()[m_swapchain.current_frame()]->handle());
		if (result != VK_SUCCESS)
			MYL_CORE_ERROR("vkQueueSubmit failed, result: {}", VkResult_to_string(result));
		
		command_buffer.update_submitted();
		
		// Give the image back to the swapchain
		m_swapchain.present(m_context.device().graphics_queue(), m_context.device().present_queue(), m_swapchain.queue_complete_semaphores()[m_swapchain.current_frame()], m_context.image_index());
	}

	std::shared_ptr<render::shader> backend::create_shader(const std::filesystem::path& a_file) {
		return std::make_shared<vulkane::shader>(m_context, a_file, m_swapchain);
	}

	void backend::on_window_resize(const u32vec2& a_size) {
		m_swapchain.recreate(a_size.w, a_size.h);
	}
}
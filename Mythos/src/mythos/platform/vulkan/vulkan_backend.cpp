#include "vulkan_backend.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_shader.hpp"

#include <mythos/core/log.hpp>
#include <mythos/core/app.hpp>
#include <mythos/math/vec3.hpp>

namespace myl::vulkan {
	backend::backend()
		: m_context()
		, m_cached_window_extent(VkExtent2D(app::get().window()->size().x, app::get().window()->size().y))
		, m_swapchain(m_context, nullptr, m_cached_window_extent)
		, m_main_render_pass(m_context, m_swapchain, f32vec4{ .2f, .2f, .2f, 1.f }, 0.f, 0.f, m_swapchain.extent(), 1.f, 0) {
		m_swapchain.set_render_pass(&m_main_render_pass);
		m_swapchain.regenerate_framebuffers(m_main_render_pass.extent());

		m_context.create_command_buffers(m_swapchain);

		m_shader = create_shader("resources/shaders/shader.glsl");

		/// MYTemp: Test geometry
		const f32 scale = 10.f;

		const u32 vert_count = 4;
		f32vec3 verts[vert_count]{
			f32vec3{ -.5f, -.5f, 0.f } * scale,
			f32vec3{ .5f, .5f, 0.f } * scale,
			f32vec3{ -.5f, .5f, 0.f } * scale,
			f32vec3{ .5f, -.5f, 0.f } * scale
		};

		const u32 index_count = 6;
		u32 indices[index_count] = { 0, 1, 2, 0, 3, 1 };

		m_context.upload(m_context.graphics_cmd_pool(), 0, m_context.graphics_queue(), *m_context.vertex_buffer(), 0, sizeof(f32vec3) * vert_count, verts);
		m_context.upload(m_context.graphics_cmd_pool(), 0, m_context.graphics_queue(), *m_context.index_buffer(), 0, sizeof(u32) * index_count, indices);
		/// End of temp code
	}

	backend::~backend() {
		vkDeviceWaitIdle(m_context.device());
		m_shader.reset();

		// C++ standard requires that members in a destructor be destoryed in the opposite order of construction, therefore
		// Destroyed m_main_render_pass
		// Destroyed m_swapchain
		// Destroyed m_context
	}

	bool backend::begin() {
		// Check if recreating swap chain and boot out.
		if (m_swapchain.recreating()) {
			VkResult result = vkDeviceWaitIdle(m_context.device());
			if (!result_is_success(result)) {
				MYL_CORE_ERROR("Vulkan backend::begin vkDeviceWaitIdle (1) failed: '{}'", VkResult_to_string(result, true));
				return false;
			}
			MYL_CORE_INFO("Recreating swapchain, exiting render loop");
			return false;
		}

		// Check if the framebuffer has been resized. If so, a new swapchain must be created.
		if (m_cached_window_extent.width != m_swapchain.extent().width || m_cached_window_extent.height != m_swapchain.extent().height) {
			VkResult result = vkDeviceWaitIdle(m_context.device());
			if (!result_is_success(result)) {
				MYL_CORE_ERROR("Vulkan backend::begin vkDeviceWaitIdle (2) failed: '{}'", VkResult_to_string(result, true));
				return false;
			}
		
			// If the swapchain recreation failed (because, for example, the window was minimized), boot out before unsetting the flag.
			if (!m_swapchain.recreate(m_cached_window_extent))
				return false;
		
			MYL_CORE_INFO("Resized, booting.");
			return false;
		}
		
		if (!m_swapchain.in_flight_fences()[m_swapchain.current_frame()]->wait()) {
			MYL_CORE_WARN("In-flight fence wait failure");
			return false;
		}

		// Acquire the next image from the swap chain. Pass along the semaphore that should signaled when this completes.
		// This same semaphore will later be waited on by the queue submission to ensure this image is available.
		if (!m_swapchain.acquire_next_image_index(UINT64_MAX, m_swapchain.image_available_semaphores()[m_swapchain.current_frame()], VK_NULL_HANDLE, &m_context.image_index()))
			return false;

		// Begin recording commands.
		command_buffer& cmd_buffer = m_context.graphics_command_buffers()[m_context.image_index()];
		cmd_buffer.reset();
		cmd_buffer.begin(false, false, false);

		// Dynamic state
		VkViewport viewport{
			.x = 0.f,
			.y = static_cast<f32>(m_swapchain.extent().height),
			.width = static_cast<f32>(m_swapchain.extent().width),
			.height = -static_cast<f32>(m_swapchain.extent().height),
			.minDepth = 0.f,
			.maxDepth = 1.f
		};

		// Scissor
		VkRect2D scissor{
			.offset = { .x = 0, .y = 0 },
			.extent = m_swapchain.extent()
		};

		vkCmdSetViewport(cmd_buffer.handle(), 0, 1, &viewport);
		vkCmdSetScissor(cmd_buffer.handle(), 0, 1, &scissor);

		m_main_render_pass.extent() = m_swapchain.extent();

		// Begin the render pass.
		m_main_render_pass.begin(&cmd_buffer, m_swapchain.framebuffers()[m_context.image_index()].handle());

		return true;
	}

	// None of these are pointers or references so the engine does not have to wait for current frame is done rendering
	void backend::update_global_state(f32mat4x4 a_projection, f32mat4x4 a_view, f32vec3 a_position, f32vec4 a_color, i32 a_mode) {
		command_buffer& cmd_buf = m_context.graphics_command_buffers()[m_context.image_index()];
		
		auto shader = static_cast<vulkan::shader*>(m_shader.get()); /// MYTodo: Should not have to cast
		shader->bind();

		auto& global_ubo = shader->global_ubo();
		global_ubo.projection = a_projection;
		global_ubo.view = a_view;
		/// MYTodo: Other properties
		
		shader->update_global_state();
	}

	void backend::update_object(f32mat4x4 a_model) {
		static_cast<vulkan::shader*>(m_shader.get())->update(a_model);

		/// MYTemp: Test code
		m_shader->bind();

		command_buffer& cmd_buffer = m_context.graphics_command_buffers()[m_context.image_index()];
		VkDeviceSize offsets[]{ 0 };
		vkCmdBindVertexBuffers(cmd_buffer.handle(), 0, 1, &m_context.vertex_buffer()->handle(), (VkDeviceSize*)offsets);
		vkCmdBindIndexBuffer(cmd_buffer.handle(), m_context.index_buffer()->handle(), 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(cmd_buffer.handle(), 6, 1, 0, 0, 0);
		/// End of temp code
	}

	void backend::end() {
		command_buffer& cmd_buffer = m_context.graphics_command_buffers()[m_context.image_index()];

		m_main_render_pass.end(&cmd_buffer);
		cmd_buffer.end();

		// Make sure the previous frame is not using this image (i.e. its fence is being waited on)
		if (!m_swapchain.images_in_flight()[m_context.image_index()].expired())
			m_swapchain.images_in_flight()[m_context.image_index()].lock()->wait();

		// Mark the image fence as in-use by this frame.
		m_swapchain.images_in_flight()[m_context.image_index()] = m_swapchain.in_flight_fences()[m_swapchain.current_frame()];


		// Reset the fence for use on the next frame
		m_swapchain.in_flight_fences()[m_swapchain.current_frame()]->reset();

		// Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
		// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
		// writes from executing until the semaphore signals (i.e. one frame is presented at a time)
		VkPipelineStageFlags flags[]{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		// Submit the queue and wait for the operation to complete.
		// Begin queue submission
		VkSubmitInfo submit_info = {
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			// Wait semaphore ensures that the operation cannot begin until the image is available.
			.pWaitSemaphores = &m_swapchain.image_available_semaphores()[m_swapchain.current_frame()],
			.pWaitDstStageMask = flags,
			.commandBufferCount = 1,
			// Command buffer(s) to be executed.
			.pCommandBuffers = &cmd_buffer.handle(),
			.signalSemaphoreCount = 1,
			// The semaphore(s) to be signaled when the queue is complete.
			.pSignalSemaphores = &m_swapchain.queue_complete_semaphores()[m_swapchain.current_frame()]
		};

		VkResult result = vkQueueSubmit(m_context.graphics_queue(), 1, &submit_info, m_swapchain.in_flight_fences()[m_swapchain.current_frame()]->handle());
		if (result != VK_SUCCESS)
			MYL_CORE_ERROR("vkQueueSubmit failed with result: {}", VkResult_to_string(result, true));

		cmd_buffer.update_submitted();
		// End queue submission

		// Give the image back to the swapchain.
		m_swapchain.present(m_context.graphics_queue(), m_context.present_queue(), m_swapchain.queue_complete_semaphores()[m_swapchain.current_frame()], m_context.image_index());
	}

	void backend::on_window_resize(const u32vec2& a_size) {
		m_cached_window_extent = VkExtent2D{ .width = a_size.w, .height = a_size.h };
	}

	std::shared_ptr<render::shader> backend::create_shader(const std::filesystem::path& a_file) {
		return std::make_shared<vulkan::shader>(m_context, a_file, m_swapchain, m_main_render_pass);
	}
}
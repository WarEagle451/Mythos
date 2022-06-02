#include "vulkan_backend.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_command_buffer.hpp"

#include <mythos/core/log.hpp>
#include <mythos/math/vec3.hpp>

bool recreate_swapchain(myl::vulkan::context& context, myl::vulkan::swapchain& swapchain, myl::vulkan::render_pass& main_render_pass) {
	// If already being recreated, do not try again.
	if (swapchain.recreating()) {
		MYL_CORE_DEBUG("recreate_swapchain called when already recreating. Booting.");
		return false;
	}

	// Detect if the window is too small to be drawn to
	if (context.framebuffer_extent().width == 0 || context.framebuffer_extent().height == 0) {
		MYL_CORE_DEBUG("recreate_swapchain called when window is < 1 in a dimension. Booting.");
		return false;
	}

	// Mark as recreating if the dimensions are valid.
	swapchain.recreating() = true;

	// Wait for any operations to complete.
	vkDeviceWaitIdle(context.device());

	// Clear these out just in case.
	for (myl::u32 i = 0; i < swapchain.images().size(); ++i) {
		swapchain.images_in_flight()[i].reset();
	}

	// Requery support
	context.query_swapchain_support();
	context.detect_depth_format();

	swapchain.recreate(context.cached_framebuffer_extent());

	// Sync the framebuffer size with the cached sizes.
	context.framebuffer_extent() = context.cached_framebuffer_extent();
	main_render_pass.extent() = context.framebuffer_extent();
	context.cached_framebuffer_extent() = VkExtent2D{ 0, 0 };

	// Update framebuffer size generation.
	context.framebuffer_size_last_generation() = context.framebuffer_size_generation();

	// cleanup swapchain
	for (auto& buf : context.graphics_command_buffers())
		buf.deallocate(context.graphics_cmd_pool());

	// Framebuffers.
	swapchain.framebuffers().clear();

	main_render_pass.x() = 0;
	main_render_pass.y() = 0;
	main_render_pass.extent() = context.framebuffer_extent();

	swapchain.regenerate_framebuffers(main_render_pass.extent());

	context.create_command_buffers(swapchain);

	// Clear the recreating flag.
	swapchain.recreating() = false;

	return true;
}

namespace myl::vulkan {
	backend::backend()
		: m_context()
		, m_swapchain(m_context, nullptr, m_context.framebuffer_extent())
		, m_main_render_pass(m_context, m_swapchain, f32vec4{ .2f, .2f, .2f, 1.f }, 0.f, 0.f, m_context.framebuffer_extent(), 1.f, 0) {
		m_swapchain.set_render_pass(&m_main_render_pass);
		m_swapchain.regenerate_framebuffers(m_main_render_pass.extent());
		MYL_CORE_INFO("Created framebuffers");

		m_context.create_command_buffers(m_swapchain);
		MYL_CORE_INFO("Created command buffers");

		m_shader = std::make_unique<shader>(m_context, "resources/shaders/shader.glsl", m_swapchain, m_main_render_pass); /// MYTemp

		/// MYTODO: temporary test code
		const u32 vert_count = 4;
		f32vec3 verts[vert_count]{
			{ 0.f, -.5f, 0.f },
			{ .5f, .5f, 0.f },
			{ 0.f, .5f, 0.f },
			{ .5f, -.5f, 0.f }
		};

		const u32 index_count = 6;
		u32 indices[index_count] = { 0, 1, 2, 0, 3, 1 };

		m_context.upload(m_context.graphics_cmd_pool(), 0, m_context.graphics_queue(), *m_context.vertex_buffer(), 0, sizeof(f32vec3) * vert_count, verts);
		m_context.upload(m_context.graphics_cmd_pool(), 0, m_context.graphics_queue(), *m_context.index_buffer(), 0, sizeof(u32) * index_count, indices);
		/// MYTODO: end temp code

		MYL_CORE_INFO("Created Vulkan backend");
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
				MYL_CORE_ERROR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (1) failed: '{}'", VkResult_to_string(result, true));
				return false;
			}
			MYL_CORE_INFO("Recreating swapchain, booting.");
			return false;
		}

		// Check if the framebuffer has been resized. If so, a new swapchain must be created.
		if (m_context.framebuffer_size_generation() != m_context.framebuffer_size_last_generation()) {
			VkResult result = vkDeviceWaitIdle(m_context.device());
			if (!result_is_success(result)) {
				MYL_CORE_ERROR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (2) failed: '{}'", VkResult_to_string(result, true));
				return false;
			}

			// If the swapchain recreation failed (because, for example, the window was minimized),
			// boot out before unsetting the flag.
			if (!recreate_swapchain(m_context, m_swapchain, m_main_render_pass)) {
				return false;
			}

			MYL_CORE_INFO("Resized, booting.");
			return false;
		}

		if (!m_swapchain.in_flight_fences()[m_swapchain.current_frame()]->wait()) {
			MYL_CORE_WARN("In-flight fence wait failure!");
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
			.y = static_cast<f32>(m_context.framebuffer_extent().height),
			.width = static_cast<f32>(m_context.framebuffer_extent().width),
			.height = -static_cast<f32>(m_context.framebuffer_extent().height),
			.minDepth = 0.f,
			.maxDepth = 1.f
		};

		// Scissor
		VkRect2D scissor{
			.offset = {.x = 0, .y = 0 },
			.extent = m_context.framebuffer_extent()
		};

		vkCmdSetViewport(cmd_buffer.handle(), 0, 1, &viewport);
		vkCmdSetScissor(cmd_buffer.handle(), 0, 1, &scissor);

		m_main_render_pass.extent() = m_context.framebuffer_extent();

		// Begin the render pass.
		m_main_render_pass.begin(&cmd_buffer, m_swapchain.framebuffers()[m_context.image_index()].handle());
		// TODO: temporary test code
		m_shader->bind();

		// Bind vertex buffer at offset.
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(cmd_buffer.handle(), 0, 1, &m_context.vertex_buffer()->handle(), (VkDeviceSize*)offsets);

		// Bind index buffer at offset.
		vkCmdBindIndexBuffer(cmd_buffer.handle(), m_context.index_buffer()->handle(), 0, VK_INDEX_TYPE_UINT32);

		// Issue the draw.
		vkCmdDrawIndexed(cmd_buffer.handle(), 6, 1, 0, 0, 0);
		// TODO: end temporary test code

		return true;
	}

	void backend::end() {
		command_buffer& cmd_buffer = m_context.graphics_command_buffers()[m_context.image_index()];

		// End renderpass
		m_main_render_pass.end(&cmd_buffer);

		cmd_buffer.end();

		// Make sure the previous frame is not using this image (i.e. its fence is being waited on)
		if (!m_swapchain.images_in_flight()[m_context.image_index()].expired())
			m_swapchain.images_in_flight()[m_context.image_index()].lock()->wait();

		// Mark the image fence as in-use by this frame.
		m_swapchain.images_in_flight()[m_context.image_index()] = m_swapchain.in_flight_fences()[m_swapchain.current_frame()];


		// Reset the fence for use on the next frame
		m_swapchain.in_flight_fences()[m_swapchain.current_frame()]->reset();

		// Submit the queue and wait for the operation to complete.
		// Begin queue submission
		VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

		// Command buffer(s) to be executed.
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cmd_buffer.handle();

		// The semaphore(s) to be signaled when the queue is complete.
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &m_swapchain.queue_complete_semaphores()[m_swapchain.current_frame()];

		// Wait semaphore ensures that the operation cannot begin until the image is available.
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &m_swapchain.image_available_semaphores()[m_swapchain.current_frame()];

		// Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
		// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
		// writes from executing until the semaphore signals (i.e. one frame is presented at a time)
		VkPipelineStageFlags flags[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_info.pWaitDstStageMask = flags;

		VkResult result = vkQueueSubmit(
			m_context.graphics_queue(),
			1,
			&submit_info,
			m_swapchain.in_flight_fences()[m_swapchain.current_frame()]->handle());
		if (result != VK_SUCCESS) {
			MYL_CORE_ERROR("vkQueueSubmit failed with result: {}", VkResult_to_string(result, true));
		}

		cmd_buffer.update_submitted();
		// End queue submission

		// Give the image back to the swapchain.
		m_swapchain.present(m_context.graphics_queue(), m_context.present_queue(), m_swapchain.queue_complete_semaphores()[m_swapchain.current_frame()], m_context.image_index());
	}

	void backend::on_window_resize(const u32vec2& a_size) {
		// Update the "framebuffer size generation", a counter which indicates when the
		// framebuffer size has been updated.
		m_context.cached_framebuffer_extent() = VkExtent2D{ .width = a_size.w, .height = a_size.h };
		m_context.framebuffer_size_generation()++;
	}

	std::shared_ptr<render::shader> backend::create_shader(const std::filesystem::path& a_file) {
		return std::make_shared<shader>(m_context, a_file, m_swapchain, m_main_render_pass);
	}
}
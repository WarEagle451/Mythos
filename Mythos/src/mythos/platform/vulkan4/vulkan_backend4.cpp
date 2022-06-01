#include "vulkan_backend.hpp"
#include "vulkan_utils.hpp"
#include "vulkan_command_buffer.hpp"

#include <mythos/core/log.hpp>
#include <mythos/math/vec3.hpp>

void upload_data_range(myl::vulkan4::context* context, VkCommandPool pool, VkFence fence, VkQueue queue, myl::vulkan4::buffer* buffer, myl::u64 offset, myl::u64 size, void* data) {
	// Create a host-visible staging buffer to upload to. Mark it as the source of the transfer.
	VkBufferUsageFlags flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	myl::vulkan4::buffer staging(*context, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, flags, true);

	// Load the data into the staging buffer.
	staging.load(0, size, 0, data);

	// Perform the copy from staging to the device local buffer.
	staging.copy_to(buffer->handle(), pool, fence, queue, offset, size);
}

void create_command_buffers(myl::vulkan4::context& context) {
	context.graphics_command_buffers.clear();
	if (context.graphics_command_buffers.empty())
		for (myl::u32 i = 0; i != context.swapchain->images().size(); ++i)
			context.graphics_command_buffers.push_back(myl::vulkan4::command_buffer(context));

	for (auto& buf : context.graphics_command_buffers)
		buf.allocate(context.m_graphics_command_pool, true);

	MYL_CORE_DEBUG("Vulkan command buffers created.");
}

void regenerate_framebuffers(myl::vulkan4::context& context, myl::vulkan4::swapchain* swapchain, myl::vulkan4::render_pass* renderpass) { /// MYTodo: Should probs have a regernate framebuffer thing
	context.swapchain->framebuffers().clear();
	context.swapchain->framebuffers().reserve(swapchain->images().size()); // If reserve does not happen, when context.swapchain.framebuffers.emplace_back tries to resize, it will crash
	for (myl::u32 i = 0; i != swapchain->images().size(); ++i) {
		// TODO: make this dynamic based on the currently configured attachments
		myl::u32 attachment_count = 2;
		std::vector<VkImageView> attachments{
			swapchain->views()[i],
			swapchain->depth_attachment().view()
		};

		context.swapchain->framebuffers().emplace_back(context, *renderpass, context.m_framebuffer_width, context.m_framebuffer_height, attachments);
	}
}

bool recreate_swapchain(myl::vulkan4::context& context) {
	// If already being recreated, do not try again.
	if (context.recreating_swapchain) {
		MYL_CORE_DEBUG("recreate_swapchain called when already recreating. Booting.");
		return false;
	}

	// Detect if the window is too small to be drawn to
	if (context.m_framebuffer_width == 0 || context.m_framebuffer_height == 0) {
		MYL_CORE_DEBUG("recreate_swapchain called when window is < 1 in a dimension. Booting.");
		return false;
	}

	// Mark as recreating if the dimensions are valid.
	context.recreating_swapchain = true;

	// Wait for any operations to complete.
	vkDeviceWaitIdle(context.m_device);

	// Clear these out just in case.
	for (myl::u32 i = 0; i < context.swapchain->images().size(); ++i) {
		context.images_in_flight[i].reset();
	}

	// Requery support
	context.query_swapchain_support();
	context.detect_depth_format();

	context.swapchain->recreate(context.cached_framebuffer_width, context.cached_framebuffer_height);

	// Sync the framebuffer size with the cached sizes.
	context.m_framebuffer_width = context.cached_framebuffer_width;
	context.m_framebuffer_height = context.cached_framebuffer_height;
	context.main_renderpass->m_w = context.m_framebuffer_width;
	context.main_renderpass->m_h = context.m_framebuffer_height;
	context.cached_framebuffer_width = 0;
	context.cached_framebuffer_height = 0;

	// Update framebuffer size generation.
	context.m_framebuffer_size_last_generation = context.m_framebuffer_size_generation;

	// cleanup swapchain
	for (auto& buf : context.graphics_command_buffers)
		buf.deallocate(context.m_graphics_command_pool);

	// Framebuffers.
	context.swapchain->framebuffers().clear();

	context.main_renderpass->m_x = 0;
	context.main_renderpass->m_y = 0;
	context.main_renderpass->m_w = context.m_framebuffer_width;
	context.main_renderpass->m_h = context.m_framebuffer_height;

	regenerate_framebuffers(context, context.swapchain.get(), context.main_renderpass.get());

	create_command_buffers(context);

	// Clear the recreating flag.
	context.recreating_swapchain = false;

	return true;
}

bool create_buffers(myl::vulkan4::context* context) {
	VkMemoryPropertyFlagBits memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	myl::u64 vertex_buffer_size = sizeof(myl::f32vec3) * 1024 * 1024;
	context->object_vertex_buffer = std::make_unique<myl::vulkan4::buffer>(
		*context,
		vertex_buffer_size,
		static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
		memory_property_flags,
		true);
	context->geometry_vertex_offset = 0;

	const myl::u64 index_buffer_size = sizeof(myl::u32) * 1024 * 1024;
	context->object_index_buffer = std::make_unique<myl::vulkan4::buffer>(
		*context,
		index_buffer_size,
		static_cast<VkBufferUsageFlagBits>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT),
		memory_property_flags,
		true);
	context->geometry_index_offset = 0;

	return true;
}

namespace myl::vulkan4 {
	backend::backend()
		: m_context() {
		// Swapchain
		m_context.swapchain = std::make_unique<myl::vulkan4::swapchain>(m_context, m_context.m_framebuffer_width, m_context.m_framebuffer_height);

		m_context.main_renderpass = std::make_unique<render_pass>(m_context, f32vec4{ .2f, .2f, .2f, 1.f }, 0.f, 0.f, m_context.m_framebuffer_width, m_context.m_framebuffer_height, 1.f, 0);

		// Swapchain framebuffers.
		regenerate_framebuffers(m_context, m_context.swapchain.get(), m_context.main_renderpass.get());

		// Create command buffers.
		create_command_buffers(m_context);

		// Create sync objects.
		m_context.image_available_semaphores.resize(m_context.swapchain->max_frames_in_flight());
		m_context.queue_complete_semaphores.resize(m_context.swapchain->max_frames_in_flight());
		m_context.in_flight_fences.resize(m_context.swapchain->max_frames_in_flight());

		for (u8 i = 0; i < m_context.swapchain->max_frames_in_flight(); ++i) {
			VkSemaphoreCreateInfo semaphore_create_info = { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
			vkCreateSemaphore(m_context.m_device, &semaphore_create_info, VK_NULL_HANDLE, &m_context.image_available_semaphores[i]);
			vkCreateSemaphore(m_context.m_device, &semaphore_create_info, VK_NULL_HANDLE, &m_context.queue_complete_semaphores[i]);

			// Create the fence in a signaled state, indicating that the first frame has already been "rendered".
			// This will prevent the application from waiting indefinitely for the first frame to render since it
			// cannot be rendered until a frame is "rendered" before it.
			m_context.in_flight_fences[i] = std::make_shared<vulkan4::fence>(m_context, true);
		}

		// In flight fences should not yet exist at this point, so clear the list. These are stored in pointers
		// because the initial state should be 0, and will be 0 when not in use. Acutal fences are not owned
		// by this list.
		m_context.images_in_flight.resize(m_context.swapchain->images().size());

		// Create builtin shaders

		m_context.object_shader = std::make_unique<myl::vulkan4::shader>(m_context, "resources/shaders/shader.glsl", *m_context.swapchain);

		create_buffers(&m_context);

		// TODO: temporary test code
		const myl::u32 vert_count = 4;
		myl::f32vec3 verts[vert_count]{
			{ 0.f, -.5f, 0.f },
			{ .5f, .5f, 0.f },
			{ 0.f, .5f, 0.f },
			{ .5f, -.5f, 0.f }
		};

		const myl::u32 index_count = 6;
		myl::u32 indices[index_count] = { 0, 1, 2, 0, 3, 1 };

		upload_data_range(&m_context, m_context.m_graphics_command_pool, 0, m_context.m_graphics_queue, m_context.object_vertex_buffer.get(), 0, sizeof(myl::f32vec3) * vert_count, verts);
		upload_data_range(&m_context, m_context.m_graphics_command_pool, 0, m_context.m_graphics_queue, m_context.object_index_buffer.get(), 0, sizeof(myl::u32) * index_count, indices);
		// TODO: end temp code

		MYL_CORE_INFO("Vulkan renderer initialized successfully.");
	}

	backend::~backend() {
		vkDeviceWaitIdle(m_context.m_device);

		// Destroy in the opposite order of creation.
		// Destroy buffers
		m_context.object_vertex_buffer.reset();
		m_context.object_index_buffer.reset();

		m_context.object_shader.reset();

		// Sync objects
		for (u8 i = 0; i < m_context.swapchain->max_frames_in_flight(); ++i) {
			if (m_context.image_available_semaphores[i]) {
				vkDestroySemaphore(
					m_context.m_device,
					m_context.image_available_semaphores[i],
					VK_NULL_HANDLE);
				m_context.image_available_semaphores[i] = 0;
			}
			if (m_context.queue_complete_semaphores[i]) {
				vkDestroySemaphore(
					m_context.m_device,
					m_context.queue_complete_semaphores[i],
					VK_NULL_HANDLE);
				m_context.queue_complete_semaphores[i] = 0;
			}
			m_context.in_flight_fences[i].reset();
		}

		// Command buffers
		for (auto& buf : m_context.graphics_command_buffers) {
			if (buf.handle())
				buf.deallocate(m_context.m_graphics_command_pool);
		}

		// Destroy framebuffers
		m_context.swapchain->framebuffers().clear();

		// Renderpass
		m_context.main_renderpass.reset();

		// Swapchain
		m_context.swapchain.reset();

		/// Destroy of context
	}

	bool backend::begin() {
		// Check if recreating swap chain and boot out.
		if (m_context.recreating_swapchain) {
			VkResult result = vkDeviceWaitIdle(m_context.m_device);
			if (!myl::vulkan4::result_is_success(result)) {
				MYL_CORE_ERROR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (1) failed: '{}'", myl::vulkan4::VkResult_to_string(result, true));
				return false;
			}
			MYL_CORE_INFO("Recreating swapchain, booting.");
			return false;
		}

		// Check if the framebuffer has been resized. If so, a new swapchain must be created.
		if (m_context.m_framebuffer_size_generation != m_context.m_framebuffer_size_last_generation) {
			VkResult result = vkDeviceWaitIdle(m_context.m_device);
			if (!myl::vulkan4::result_is_success(result)) {
				MYL_CORE_ERROR("vulkan_renderer_backend_begin_frame vkDeviceWaitIdle (2) failed: '{}'", myl::vulkan4::VkResult_to_string(result, true));
				return false;
			}

			// If the swapchain recreation failed (because, for example, the window was minimized),
			// boot out before unsetting the flag.
			if (!recreate_swapchain(m_context)) {
				return false;
			}

			MYL_CORE_INFO("Resized, booting.");
			return false;
		}

		if (!m_context.in_flight_fences[m_context.m_current_frame]->wait()) {
			MYL_CORE_WARN("In-flight fence wait failure!");
			return false;
		}

		// Acquire the next image from the swap chain. Pass along the semaphore that should signaled when this completes.
		// This same semaphore will later be waited on by the queue submission to ensure this image is available.
		if (!m_context.swapchain->acquire_next_image_index(UINT64_MAX, m_context.image_available_semaphores[m_context.m_current_frame], VK_NULL_HANDLE, &m_context.image_index))
			return false;

		// Begin recording commands.
		command_buffer& cmd_buffer = m_context.graphics_command_buffers[m_context.image_index];
		cmd_buffer.reset();
		cmd_buffer.begin(false, false, false);

		// Dynamic state
		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = (f32)m_context.m_framebuffer_height;
		viewport.width = (f32)m_context.m_framebuffer_width;
		viewport.height = -(f32)m_context.m_framebuffer_height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		// Scissor
		VkRect2D scissor;
		scissor.offset.x = scissor.offset.y = 0;
		scissor.extent.width = m_context.m_framebuffer_width;
		scissor.extent.height = m_context.m_framebuffer_height;

		vkCmdSetViewport(cmd_buffer.handle(), 0, 1, &viewport);
		vkCmdSetScissor(cmd_buffer.handle(), 0, 1, &scissor);

		m_context.main_renderpass->m_w = m_context.m_framebuffer_width;
		m_context.main_renderpass->m_h = m_context.m_framebuffer_height;

		// Begin the render pass.
		m_context.main_renderpass->begin(&cmd_buffer, m_context.swapchain->framebuffers()[m_context.image_index].handle());
		// TODO: temporary test code
		m_context.object_shader->bind();

		// Bind vertex buffer at offset.
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(cmd_buffer.handle(), 0, 1, &m_context.object_vertex_buffer->handle(), (VkDeviceSize*)offsets);

		// Bind index buffer at offset.
		vkCmdBindIndexBuffer(cmd_buffer.handle(), m_context.object_index_buffer->handle(), 0, VK_INDEX_TYPE_UINT32);

		// Issue the draw.
		vkCmdDrawIndexed(cmd_buffer.handle(), 6, 1, 0, 0, 0);
		// TODO: end temporary test code

		return true;
	}

	void backend::end() {
		command_buffer& cmd_buffer = m_context.graphics_command_buffers[m_context.image_index];

		// End renderpass
		m_context.main_renderpass->end(&cmd_buffer);

		cmd_buffer.end();

		// Make sure the previous frame is not using this image (i.e. its fence is being waited on)
		if (!m_context.images_in_flight[m_context.image_index].expired())
			m_context.images_in_flight[m_context.image_index].lock()->wait();

		// Mark the image fence as in-use by this frame.
		m_context.images_in_flight[m_context.image_index] = m_context.in_flight_fences[m_context.m_current_frame];


		// Reset the fence for use on the next frame
		m_context.in_flight_fences[m_context.m_current_frame]->reset();

		// Submit the queue and wait for the operation to complete.
		// Begin queue submission
		VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };

		// Command buffer(s) to be executed.
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &cmd_buffer.handle();

		// The semaphore(s) to be signaled when the queue is complete.
		submit_info.signalSemaphoreCount = 1;
		submit_info.pSignalSemaphores = &m_context.queue_complete_semaphores[m_context.m_current_frame];

		// Wait semaphore ensures that the operation cannot begin until the image is available.
		submit_info.waitSemaphoreCount = 1;
		submit_info.pWaitSemaphores = &m_context.image_available_semaphores[m_context.m_current_frame];

		// Each semaphore waits on the corresponding pipeline stage to complete. 1:1 ratio.
		// VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT prevents subsequent colour attachment
		// writes from executing until the semaphore signals (i.e. one frame is presented at a time)
		VkPipelineStageFlags flags[1] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submit_info.pWaitDstStageMask = flags;

		VkResult result = vkQueueSubmit(
			m_context.m_graphics_queue,
			1,
			&submit_info,
			m_context.in_flight_fences[m_context.m_current_frame]->handle());
		if (result != VK_SUCCESS) {
			MYL_CORE_ERROR("vkQueueSubmit failed with result: {}", myl::vulkan4::VkResult_to_string(result, true));
		}

		cmd_buffer.update_submitted();
		// End queue submission

		// Give the image back to the swapchain.
		m_context.swapchain->present(m_context.m_graphics_queue, m_context.m_present_queue, m_context.queue_complete_semaphores[m_context.m_current_frame], m_context.image_index);
	}

	void backend::on_window_resize(const u32vec2& a_size) {
		// Update the "framebuffer size generation", a counter which indicates when the
		// framebuffer size has been updated.
		m_context.cached_framebuffer_width = a_size.w;
		m_context.cached_framebuffer_height = a_size.h;
		m_context.m_framebuffer_size_generation++;
	}

	std::shared_ptr<render::shader> backend::create_shader(const std::filesystem::path&) {
		return nullptr;
	}
}
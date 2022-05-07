#include "vulkan_backend.hpp"

#include <mythos/core/log.hpp>

namespace myl::vulkan {
	backend::backend()
		: m_context()
		, m_swapchain(m_context, 800, 600) { /// MYTodo: have a configurable size to start
		m_context.create_command_buffers(m_swapchain);
		MYL_CORE_INFO("Created Vulkan backend");
	}

	backend::~backend() { // C++ standard has members desructors called in opposite order of creation
		vkDeviceWaitIdle(m_context.device().logical()); // waits for all graphics operations to cease
		MYL_CORE_INFO("Destroying Vulkan backend");
	}

	bool backend::begin() {
		return true;
	}

	void backend::end() {

	}

	void backend::on_window_resize(u32 a_width, u32 a_height) {
		m_swapchain.recreate(a_width, a_height);
	}
}
#include "vulkan_backend.hpp"

#include <mythos/core/log.hpp>

namespace myl::vulkan {
	backend::backend(const app::info& a_info)
		: m_context(a_info) {
		// Order of creation:
		// context: Instance, *debugger, surface, ->
		//	- device: physical device, logical device
		//	- swapchain:
		MYL_CORE_INFO("Vulkan backend initialized");
	}

	backend::~backend() {
		// C++ standard has members desructors called in opposite order of creation
		MYL_CORE_DEBUG("Destorying Vulkan backend");
	}

	bool backend::begin() {
		return true;
	}

	void backend::end() {

	}

	void backend::on_window_resize(u32 a_width, u32 a_height) {
		m_context.swapchain().recreate(a_width, a_height);
	}
}
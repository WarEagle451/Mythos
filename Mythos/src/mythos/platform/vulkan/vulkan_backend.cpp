#include "vulkan_backend.hpp"

#include <mythos/core/log.hpp>

namespace myl::vulkan {
	backend::backend(const app::info& a_info)
		: m_context(a_info) {
		// Order of creation:
		// context: Instance, *debugger, surface, ->
		//	- device: select physical device, logical device
		//	- swapchain:
		MYL_CORE_INFO("Vulkan backend initialized");
	}

	backend::~backend() {
		MYL_CORE_DEBUG("Destorying Vulkan backend");
	}

	bool backend::begin() {
		return true;
	}

	void backend::end() {

	}

	void backend::on_window_resize(u32 a_width, u32 a_height) {

	}
}
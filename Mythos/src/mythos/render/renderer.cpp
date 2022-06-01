#include "renderer.hpp"

#include <mythos/platform/detection.hpp>

#include <mythos/core/log.hpp>

namespace myl::render {
	std::unique_ptr<backend> renderer::s_backend = nullptr;
	api renderer::s_api = api::none;

	static constexpr api choose_renderer_api() {
#ifdef MYL_PLATFORM_WINDOWS
		return api::vulkan;
#else
		/// MYTodo: Should attempt opengl as last resort
#endif
	}

	void renderer::init(render::api a_api) {
		s_api = a_api == api::none ? choose_renderer_api() : a_api;
		s_backend = backend::create(s_api);
	}

	void renderer::shutdown() {
		/// MYTodo: All shaders must be destroyed before backend is destroyed
		/// MYTodo: Should probs unbind the shader being used if there is one

		s_backend.reset(); // Deletes the backend
	}

	void renderer::draw_frame() { /// MYTodo: This should draw all of the submited data using the draw functions, aka make it a batch renderer
		if (s_backend->begin()) {
			s_backend->end();
		}
	}

	void renderer::on_window_resize(const u32vec2& a_size) {
		s_backend->on_window_resize(a_size);
	}
}
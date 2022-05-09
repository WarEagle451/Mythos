#include "renderer.hpp"

namespace myl::render {
	std::unique_ptr<backend> renderer::s_backend = nullptr;
	api renderer::s_api = api::vulkan; /// MYTodo: Needs to be able to detect render api

	void renderer::init() {
		s_backend = backend::create(s_api);
	}

	void renderer::shutdown() {
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
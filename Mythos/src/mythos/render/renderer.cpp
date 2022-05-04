#include "renderer.hpp"

namespace myl::render {
	std::unique_ptr<backend> renderer::s_backend = nullptr;
	api renderer::s_api = api::vulkan; /// MYTodo: Needs to be able to detect render api

	void renderer::init(const app::info& a_info) {
		s_backend = backend::create(s_api, a_info);
	}

	void renderer::shutdown() {
		s_backend.reset(); // deletes the backend
	}

	void renderer::draw_frame() { /// MYTodo: this should draw all of the submited data using the draw functions, aka make it a batch renderer
		if (s_backend->begin()) {
			s_backend->end();
		}
	}

	void renderer::on_resize(u32 a_width, u32 a_height) {
		s_backend->on_window_resize(a_width, a_height);
	}
}
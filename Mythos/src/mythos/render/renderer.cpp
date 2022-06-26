#include "renderer.hpp"

#include <mythos/platform/detection.hpp>
#include <mythos/core/log.hpp>
#include <mythos/math/mat4x4.hpp>

namespace myl::render {
	std::unique_ptr<backend> renderer::s_backend = nullptr;
	api renderer::s_api = api::none;
	statistics renderer::s_stats;

	struct render_data {
		f32mat4x4 view_projection;
	};

	static render_data g_data;

	static constexpr api choose_renderer_api() {
#ifdef MYL_PLATFORM_WINDOWS
		return api::vulkan;
#else

#endif
	}

	void renderer::init(render::api a_api) {
		s_api = (a_api == api::none) ? choose_renderer_api() : a_api;
		MYL_CORE_INFO("Renderer API '{}' selected", api_to_string(s_api));

		/// MYTodo: Allocate data for vertexes

		s_backend = backend::create(s_api);
	}

	void renderer::shutdown() {
		/// MYTodo: All shaders must be destroyed before backend is destroyed

		s_backend.reset();
	}

	bool renderer::begin() {
		s_stats = statistics(); // Reset stats every frame
		/// MYTodo: does not need to be bool
		/// MYTodo: This should only allow drawing to begin, not call the backend begin
		/// that should come before this in app, I think, refer to lagacy Mythos,
		/// This should just be the beginnig of a draw call
		
		///g_data.view_projection = a_camera.view();

		if (s_backend->begin())
			return true;
		return false;
	}

	void renderer::end() {
		s_backend->end(); /// MYTodo: Should not be calling this
		/// MYTodo: draw batches
		++s_stats.draw_calls;
	}

	void renderer::on_window_resize(const u32vec2& a_size) {
		s_backend->on_window_resize(a_size);
	}
}
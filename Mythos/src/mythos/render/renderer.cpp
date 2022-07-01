#include "renderer.hpp"

#include <mythos/platform/detection.hpp>
#include <mythos/core/log.hpp>

#include <mythos/math/quaternion.hpp> /// MYTemp:
#include <mythos/math/transform.hpp> /// MYTemp:

namespace myl::render {
	std::unique_ptr<backend> renderer::s_backend = nullptr;
	api renderer::s_api = api::none;
	statistics renderer::s_stats;

	static constexpr api choose_renderer_api() {
#ifdef MYL_PLATFORM_WINDOWS
		return api::vulkan;
#else

#endif
	}

	void renderer::init(render::api a_api) {
		s_api = (a_api == api::none) ? choose_renderer_api() : a_api;
		MYL_CORE_INFO("Renderer API '{}' selected", api_to_string(s_api));
		s_backend = backend::create(s_api);

		/// MYTodo: Allocate data for vertexes
	}

	void renderer::shutdown() {
		/// MYTodo: All shaders must be destroyed before backend is destroyed

		s_backend.reset();
	}

	bool renderer::begin() {
		s_stats = statistics(); // Reset stats every frame
		if (s_backend->begin())
			return true;
		return false;
	}

	void renderer::end() {
		s_backend->end();
		++s_stats.draw_calls; /// MYTodo: not here
	}

	void renderer::on_window_resize(const u32vec2& a_size) {
		s_backend->on_window_resize(a_size);
	}

	void draw_quad(const f32vec3& position, const f32vec3& rotation, const f32vec3& scale) {
		f32quat rot(forward(f32mat4x4::identity()), rotation.z, false);
		f32mat4x4 model = quat_to_rotation_matrix(rot, f32vec3::zero());
		render::renderer::backend()->update_object(model); /// MYTodo: DO NOT DO, THIS SHOULD QUEUE IT UP IN THE RENDERER
	}
}
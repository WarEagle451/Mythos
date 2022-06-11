#include "renderer.hpp"

#include <mythos/platform/detection.hpp>
#include <mythos/core/log.hpp>
#include <mythos/math.hpp>

/// MYTodo: Switch to a right handed cord system, currently left hand

namespace myl::render {
	std::unique_ptr<backend> renderer::s_backend = nullptr;
	api renderer::s_api = api::none;

	static constexpr api choose_renderer_api() {
#ifdef MYL_PLATFORM_WINDOWS
		return api::vulkan;
#else

#endif
	}

	void renderer::init(render::api a_api) {
		s_api = a_api == api::none ? choose_renderer_api() : a_api;
		s_backend = backend::create(s_api);
	}

	void renderer::shutdown() {
		/// MYTodo: All shaders must be destroyed before backend is destroyed

		s_backend.reset();
	}

	static f32 z = 20.f; /// MYTemp:
	static bool flip = false; /// MYTemp:
	static f32 angle = 0.f; /// MYTemp:

	void renderer::draw_frame() {
		if (s_backend->begin()) {
			f32mat4x4 projection = perspective(radians(45.f), app::get().window()->aspect_ratio(), .1f, 1000.f); /// MYTemp: The screen
			f32mat4x4 view = inverse(translation(f32vec3{ 0, 0, z })); /// MYTemp: Camera
			MYL_CORE_DEBUG(z);
			f32quat rot(forward(f32mat4x4::identity()), angle, false);
			f32mat4x4 model = quat_to_rotation_matrix(rot, f32vec3::zero()); /// MYTemp: Position

			if (z > 45.f || z < 15.f)
				flip = !flip;

			flip ? z -= .2f : z += .2f;
			angle += .01f;

			s_backend->update_global_state(projection, view, f32vec3::zero(), f32vec4::one(), 0); /// MYTemp:
			s_backend->update_object(model); /// MYTodo: hate this style, this should be draw_quad, draw_model
			s_backend->end();
		}
	}

	void renderer::on_window_resize(const u32vec2& a_size) {
		s_backend->on_window_resize(a_size);
	}
}
#include "scene.hpp"

#include <mythos/algorithm.hpp> /// MYTemp:
#include <mythos/core/app.hpp> /// MYTemp:
#include <mythos/input.hpp> /// MYTemp:
#include <mythos/render/renderer.hpp>

namespace myl {
	void camera::update() { /// MYTemp:
		m_aspect = app::get().window()->aspect_ratio();
		m_viewport = app::get().window()->size();

		f32vec3 velocity = f32vec3::zero();
		if (input::key_down(key::w)) velocity += forward(m_view);
		if (input::key_down(key::s)) velocity += backward(m_view);
		if (input::key_down(key::a)) velocity += left(m_view);
		if (input::key_down(key::d)) velocity += right(m_view);
		if (input::key_down(key::space)) velocity.y += 1.f;
		if (input::key_down(key::left_shift)) velocity.y -= 1.f;
		
		if (velocity != f32vec3::zero())
			m_position += normalize(velocity);

		if (input::key_down(key::q)) m_rotation.z += radians(2.f);
		if (input::key_down(key::e)) m_rotation.z -= radians(2.f);

		if (input::mouse_button_down(mouse_button::left) && input::cursor_delta() != f32vec2::zero()) {
			f32 pan_speed = 2.f;
			const auto& delta = input::cursor_delta();
			auto rotated = delta * pan_speed / length(delta);
			m_rotation.x += -radians(rotated.y);
			m_rotation.y += -radians(rotated.x);
			// Prevent Gimball lock
			constexpr f32 limit = radians(89.9f);
			m_rotation.x = clamp(m_rotation.x, -limit, limit);
		}

		recaculate_view(); /// MYTemp:
	}

	static f32 angle = 0.f; /// MYTemp:
	static f32 rotations_pre_second_rads = radians(90.f);

	void scene::update(timestep ts) {
		angle += rotations_pre_second_rads * static_cast<f32>(ts);
	}

	void scene::render() {
		if (render::renderer::begin()) {
			m_camera.update();

			/// MYTodo: Remove global state
			render::renderer::backend()->update_global_state(m_camera.projection(), m_camera.view(), f32vec3::zero(), f32vec4::one(), 0); /// MYTemp:
			render::draw_quad({ 0, 0, 0 }, { 0, 0, angle }, { 1, 1, 1 });

			render::renderer::end();
		}
	}
}
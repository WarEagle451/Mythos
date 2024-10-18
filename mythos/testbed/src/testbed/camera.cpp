#include <testbed/camera.hpp>

#include <mythos/input.hpp>

#include <myl/algorithm.hpp>
#include <myl/math/projection.hpp>
#include <myl/math/transform.hpp>
#include <myl/math/trigonometry.hpp>

#include <testbed/log.hpp>

namespace testbed {
    MYL_NO_DISCARD camera::camera(const myl::f32vec3& position, const myl::f32vec3& rotation, const myl::f32vec3& scale, myl::f32 aspect, myl::f32 fovy, myl::f32 z_near, myl::f32 z_far)
		: m_position{ position }
		, m_rotation{ rotation }
		, m_scale { scale }
        , m_aspect{ aspect }
        , m_fovy{ fovy }
        , m_near{ z_near }
        , m_far{ z_far } {

    }

    auto camera::projection() const -> myl::f32mat4 {
		auto proj = myl::perspective(myl::radians(m_fovy), m_aspect, m_near, m_far);
	    proj[1][1] *= -1; /// MYTODO: Find a way where this would not be needed
        return proj;
    }

	auto camera::transform() const -> myl::f32mat4 {
		return myl::create_transform(m_position, m_rotation, m_scale);
	}

    auto camera::on_update(myth::timestep ts) -> void {
		if (!myth::input::registered_devices().empty()) {
			auto device = static_cast<myth::hid::gamepad*>(myth::input::registered_devices().front().get());

			// Movement
			myl::f32vec3 movement(device->left_stick.x, 0.f, -device->left_stick.y);

			/// MYTODO: Implement camera roatation, need quaternions
			///myl::f32quat rotation(a_tc.rotation);

			if ((device->button_states & myth::hid_button::left_stick) == myth::hid_button::left_stick) movement.y -= 1;
			if ((device->button_states & myth::hid_button::symbol_down) == myth::hid_button::symbol_down) movement.y += 1;

			if (movement != myl::f32vec3(0.f))
				m_position += myl::normalize(movement) * .1f; // Magic number!
		}
		else { // Mouse controller
			// Zoom
			if (myl::f32 scroll = myth::input::scroll_delta().y; scroll != 0.f) {
				m_fovy -= scroll * 4.f; // Magic number!
				m_fovy = myl::clamp(m_fovy - scroll * 4.f, 1.f, 179.f);
			}

			// Movement
			myl::f32vec3 movement(0.f);
			/// MYTODO: Implement camera roatation, need quaternions
			///myl::f32quat rotation(a_tc.rotation);

			if (myth::input::key_down(myth::key::w)) movement.z += 1;/// movement += myl::forward(rotation);
			if (myth::input::key_down(myth::key::s)) movement.z -= 1;/// movement += myl::backward(rotation);
			if (myth::input::key_down(myth::key::a)) movement.x -= 1;/// movement += myl::left(rotation);
			if (myth::input::key_down(myth::key::d)) movement.x += 1;/// movement += myl::right(rotation);
			if (myth::input::key_down(myth::key::space)) movement.y += 1.f;
			if (myth::input::key_down(myth::key::left_shift)) movement.y -= 1.f;

			if (movement != myl::f32vec3(0.f))
				m_position += myl::normalize(movement) * .1f; // Magic number!

			// Rotate
			if (myth::input::mouse_buttons_down(myth::mouse_button::left)) {
				constexpr myl::f32 rotate_speed = 1.2f; // Magic number!
				myl::f32vec2 mouse_delta = myth::input::cursor_delta() * .003f; // Magic Number!
				///	myl::f32 yaw_sign = myl::sign(myl::up(rotation).y);
				///m_rotation.x += mouse_delta.y * rotate_speed;
				///m_rotation.y += yaw_sign * mouse_delta.x * rotate_speed;
			}
		}
    }

    auto camera::on_window_resize(const myl::i32vec2& dimensions) -> void {
		m_aspect = static_cast<myl::f32>(dimensions.w) / static_cast<myl::f32>(dimensions.h);
    }
}

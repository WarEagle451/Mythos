#include <testbed/camera.hpp>

#include <mythos/input.hpp>

#include <myl/algorithm.hpp>
#include <myl/math/projection.hpp>
#include <myl/math/transform.hpp>
#include <myl/math/trigonometry.hpp>

#include <testbed/log.hpp>

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

    auto camera::on_update(mye::timestep ts) -> void {
		if (!mye::input::registered_devices().empty()) {
			/// MYHACK: The first device won't always be a mouse, have some sort of check
			auto& device = mye::input::registered_devices().front();
			// Movement
			myl::f32vec3 movement(device.state.left_stick.x, 0.f, -device.state.left_stick.y);

			/// MYTODO: Implement camera roatation, need quaternions
			///myl::f32quat rotation(a_tc.rotation);

			if ((device.state.buttons & mye::gamepad_button::left_stick) == mye::gamepad_button::left_stick) movement.y -= 1;
			if ((device.state.buttons & mye::gamepad_button::action_down) == mye::gamepad_button::action_down) movement.y += 1;

			if (movement != myl::f32vec3(0.f))
				m_position += myl::normalize(movement) * .1f; // Magic number!
		}
		else { // Mouse controller
			// Zoom
			if (myl::f32 scroll = mye::input::scroll_delta().y; scroll != 0.f) {
				m_fovy -= scroll * 4.f; // Magic number!
				m_fovy = myl::clamp(m_fovy - scroll * 4.f, 1.f, 179.f);
			}

			// Movement
			myl::f32vec3 movement(0.f);
			/// MYTODO: Implement camera roatation, need quaternions
			///myl::f32quat rotation(a_tc.rotation);

			if (mye::input::key_down(mye::key::w)) movement.z += 1;/// movement += myl::forward(rotation);
			if (mye::input::key_down(mye::key::s)) movement.z -= 1;/// movement += myl::backward(rotation);
			if (mye::input::key_down(mye::key::a)) movement.x -= 1;/// movement += myl::left(rotation);
			if (mye::input::key_down(mye::key::d)) movement.x += 1;/// movement += myl::right(rotation);
			if (mye::input::key_down(mye::key::space)) movement.y += 1.f;
			if (mye::input::key_down(mye::key::left_shift)) movement.y -= 1.f;

			if (movement != myl::f32vec3(0.f))
				m_position += myl::normalize(movement) * .1f; // Magic number!			

			// Rotate
			if (mye::input::mouse_buttons_down(mye::mouse_button::left)) {
				constexpr myl::f32 rotate_speed = 1.2f; // Magic number!
				myl::f32vec2 mouse_delta = mye::input::cursor_delta() * .02f; // Magic Number!
				myl::f32 yaw_sign = myl::sign(myl::up(m_rotation).y);
				m_rotation.x += mouse_delta.y * rotate_speed;
				m_rotation.y += yaw_sign * mouse_delta.x * rotate_speed;
			}
		}
    }

    auto camera::on_window_resize(const myl::i32vec2& dimensions) -> void {
		m_aspect = static_cast<myl::f32>(dimensions.w) / static_cast<myl::f32>(dimensions.h);
    }
}

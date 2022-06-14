#pragma once
#include "event.hpp"

#include <mythos/core/mouse_codes.hpp>
#include <mythos/math/vec2.hpp>

namespace myl {
	class event_mouse_moved : public event {
		f32vec2 m_position;
	public:
		MYL_API constexpr event_mouse_moved(const f32vec2& a_position)
			: m_position(a_position){ }

		MYL_API MYL_NO_DISCARD constexpr const f32vec2& position() const { return m_position; }

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::mouse) | static_cast<i32>(event_category::input))
		MYL_IMPL_EVENT_TYPE(mouse_moved)
	};

	class event_mouse_scrolled : public event {
		f32vec2 m_delta;
	public:
		MYL_API constexpr event_mouse_scrolled(const f32 a_x_delta, const f32 a_y_delta)
			: m_delta(a_x_delta, a_y_delta) {}
		MYL_API constexpr event_mouse_scrolled(const f32vec2& a_delta)
			: m_delta(a_delta) {}

		MYL_API MYL_NO_DISCARD constexpr const f32vec2& delta() const { return m_delta; }

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::mouse) | static_cast<i32>(event_category::input))
		MYL_IMPL_EVENT_TYPE(mouse_scrolled)
	};

	class event_mouse_button : public event {
	protected:
		mouse_code m_code;

		MYL_API constexpr event_mouse_button(mouse_code a_code)
			: m_code(a_code) {}
	public:
		MYL_API MYL_NO_DISCARD constexpr mouse_code buttons() const { return m_code; }

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::mouse) | static_cast<i32>(event_category::input) | static_cast<i32>(event_category::mouse_button))
	};

	class event_mouse_pressed : public event_mouse_button {
	public:
		MYL_API constexpr event_mouse_pressed(mouse_code a_button)
			: event_mouse_button(a_button) {}

		MYL_IMPL_EVENT_TYPE(mouse_pressed)
	};

	class event_mouse_released: public event_mouse_button {
	public:
		MYL_API constexpr event_mouse_released(const mouse_code a_button)
			: event_mouse_button(a_button) {}

		MYL_IMPL_EVENT_TYPE(mouse_released)
	};
}
#pragma once
#include "event.hpp"

#include <mythos/core/mouse_codes.hpp>

namespace myl {
	class mouse_moved_event : public event {
		f32 m_x, m_y;
	public:
		MYL_API constexpr mouse_moved_event(const f32 a_x, const f32 a_y)
			: m_x(a_x)
			, m_y(a_y) {}

		MYL_API constexpr f32 x() const { return m_x; }
		MYL_API constexpr f32 y() const { return m_y; }

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::mouse) | static_cast<i32>(event_category::input))
		MYL_IMPL_EVENT_TYPE(mouse_moved)
	};

	class mouse_scrolled_event : public event {
		f32 m_x_delta, m_y_delta;
	public:
		MYL_API constexpr mouse_scrolled_event(const f32 a_x_delta, const f32 a_y_delta)
			: m_x_delta(a_x_delta)
			, m_y_delta(a_y_delta) {}

		MYL_API constexpr f32 x_delta() const { return m_x_delta; } /// MYTodo: return a vec2
		MYL_API constexpr f32 y_delta() const { return m_y_delta; }

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::mouse) | static_cast<i32>(event_category::input))
		MYL_IMPL_EVENT_TYPE(mouse_scrolled)
	};

	class mouse_button_event : public event {
	protected:
		mouse_code m_code;

		MYL_API constexpr mouse_button_event(mouse_code a_code)
			: m_code(a_code) {}
	public:
		MYL_API constexpr mouse_code button() const { return m_code; }

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::mouse) | static_cast<i32>(event_category::input) | static_cast<i32>(event_category::mouse_button))
	};

	class mouse_pressed_event : public mouse_button_event {
	public:
		MYL_API constexpr mouse_pressed_event(mouse_code a_button)
			: mouse_button_event(a_button) {}

		MYL_IMPL_EVENT_TYPE(mouse_pressed)
	};

	class mouse_released_event : public mouse_button_event {
	public:
		MYL_API constexpr mouse_released_event(const mouse_code a_button)
			: mouse_button_event(a_button) {}

		MYL_IMPL_EVENT_TYPE(mouse_released)
	};
}
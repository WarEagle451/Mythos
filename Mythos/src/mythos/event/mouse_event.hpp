#pragma once
#include "event.hpp"

#include <mythos/core/mouse_codes.hpp>
#include <mythos/math/vec2.hpp>

namespace myl::event {
	class mouse_moved : public event_base {
		f32vec2 m_position;
	public:
		MYL_API constexpr mouse_moved(const f32vec2& a_position)
			: m_position(a_position){ }

		MYL_API MYL_NO_DISCARD constexpr const f32vec2& position() const { return m_position; }

		MYL_IMPL_EVENT_CATEGORY(category::mouse | category::input)
		MYL_IMPL_EVENT_TYPE(mouse_moved)
	};

	class mouse_scrolled : public event_base {
		f32vec2 m_delta;
	public:
		MYL_API constexpr mouse_scrolled(const f32 a_x_delta, const f32 a_y_delta)
			: m_delta(a_x_delta, a_y_delta) {}
		MYL_API constexpr mouse_scrolled(const f32vec2& a_delta)
			: m_delta(a_delta) {}

		MYL_API MYL_NO_DISCARD constexpr const f32vec2& delta() const { return m_delta; }

		MYL_IMPL_EVENT_CATEGORY(category::mouse | category::input)
		MYL_IMPL_EVENT_TYPE(mouse_scrolled)
	};

	class mouse_button_event_base : public event_base {
	protected:
		mouse_code m_code;

		MYL_API constexpr mouse_button_event_base(mouse_code a_code)
			: m_code(a_code) {}
	public:
		MYL_API MYL_NO_DISCARD constexpr mouse_code buttons() const { return m_code; }

		MYL_IMPL_EVENT_CATEGORY(category::mouse | category::input | category::mouse_button)
	};

	class mouse_pressed : public mouse_button_event_base {
	public:
		MYL_API constexpr mouse_pressed(mouse_code a_button)
			: mouse_button_event_base(a_button) {}

		MYL_IMPL_EVENT_TYPE(mouse_pressed)
	};

	class mouse_released: public mouse_button_event_base {
	public:
		MYL_API constexpr mouse_released(const mouse_code a_button)
			: mouse_button_event_base(a_button) {}

		MYL_IMPL_EVENT_TYPE(mouse_released)
	};
}
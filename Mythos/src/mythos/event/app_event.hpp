#pragma once
#include "event.hpp"

#include <mythos/math/vec2.hpp>

namespace myl {
	class event_window_resize : public event {
		u32vec2 m_size;
	public:
		MYL_API event_window_resize(u32 a_width, u32 a_height)
			: m_size(a_width, a_height) {}
		MYL_API event_window_resize(const u32vec2& a_size)
			: m_size(a_size) {}

		MYL_API MYL_NO_DISCARD const u32vec2& size() const { return m_size; }

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application))
		MYL_IMPL_EVENT_TYPE(window_resize)
	};

	class event_window_close : public event {
	public:
		MYL_API event_window_close() = default;

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application))
		MYL_IMPL_EVENT_TYPE(window_close)
	};

	class event_app_tick : public event {
	public:
		MYL_API event_app_tick() = default;

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application))
		MYL_IMPL_EVENT_TYPE(app_tick)
	};

	class event_app_update: public event {
	public:
		MYL_API event_app_update() = default;

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application))
		MYL_IMPL_EVENT_TYPE(app_update)
	};

	class event_app_render : public event {
		MYL_API event_app_render() = default;

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application))
		MYL_IMPL_EVENT_TYPE(app_render)
	};
}
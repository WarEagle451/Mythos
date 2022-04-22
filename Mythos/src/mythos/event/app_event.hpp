#pragma once
#include "event.hpp"

namespace myl {
	class window_resize_event : public event {
		u32 m_width, m_height;
	public:
		MYL_API window_resize_event(u32 a_width, u32 a_height)
			: m_width(a_width)
			, m_height(a_height) {}

		MYL_API u32 width() const { return m_width; }
		MYL_API u32 height() const { return m_height; }

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application)) /// MYTodo: find a better way of implementing these macros
		MYL_IMPL_EVENT_TYPE(window_resize)
	};

	class window_close_event : public event {
	public:
		MYL_API window_close_event() = default;

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application))
		MYL_IMPL_EVENT_TYPE(window_close)
	};

	class app_tick_event : public event {
	public:
		MYL_API app_tick_event() = default;

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application))
		MYL_IMPL_EVENT_TYPE(app_tick)
	};

	class app_update_event : public event {
	public:
		MYL_API app_update_event() = default;

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application))
		MYL_IMPL_EVENT_TYPE(app_update)
	};

	class app_render_event : public event {
		MYL_API app_render_event() = default;

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::application))
		MYL_IMPL_EVENT_TYPE(app_render)
	};
}
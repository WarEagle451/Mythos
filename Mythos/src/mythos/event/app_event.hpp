#pragma once
#include "event.hpp"

#include <mythos/math/vec2.hpp>

/// Window minimize, window maximize

namespace myl {
	class event_window_resize : public event {
		u32vec2 m_dimensions;
	public:
		MYL_API event_window_resize(u32 a_width, u32 a_height)
			: m_dimensions(a_width, a_height) {}
		MYL_API event_window_resize(u32 a_dimensions)
			: m_dimensions(a_dimensions) {}

		MYL_API MYL_NO_DISCARD const u32vec2& dimensions() const { return m_dimensions; }
		MYL_API MYL_NO_DISCARD u32 width() const { return m_dimensions.x; }
		MYL_API MYL_NO_DISCARD u32 height() const { return m_dimensions.y; }

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
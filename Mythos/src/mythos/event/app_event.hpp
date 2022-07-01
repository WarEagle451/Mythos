#pragma once
#include "event.hpp"

#include <mythos/math/vec2.hpp>

namespace myl::event {
	class window_resize : public event_base {
		u32vec2 m_size;
	public:
		MYL_API window_resize(u32 a_width, u32 a_height)
			: m_size(a_width, a_height) {}
		MYL_API window_resize(const u32vec2& a_size)
			: m_size(a_size) {}

		MYL_API MYL_NO_DISCARD const u32vec2& size() const { return m_size; }

		MYL_IMPL_EVENT_CATEGORY(category::application)
		MYL_IMPL_EVENT_TYPE(window_resize)
	};

	class window_close : public event_base {
	public:
		MYL_API window_close() = default;

		MYL_IMPL_EVENT_CATEGORY(category::application)
		MYL_IMPL_EVENT_TYPE(window_close)
	};

	class app_tick : public event_base {
	public:
		MYL_API app_tick() = default;

		MYL_IMPL_EVENT_CATEGORY(category::application)
		MYL_IMPL_EVENT_TYPE(app_tick)
	};

	class app_update: public event_base {
	public:
		MYL_API app_update() = default;

		MYL_IMPL_EVENT_CATEGORY(category::application)
		MYL_IMPL_EVENT_TYPE(app_update)
	};

	class app_render : public event_base {
		MYL_API app_render() = default;

		MYL_IMPL_EVENT_CATEGORY(category::application)
		MYL_IMPL_EVENT_TYPE(app_render)
	};
}
#pragma once
#include <mythos/defines.hpp>

/// MYTodo: I don't like this macro
#include <memory> /// for macro below
#define MYL_BIND_EVENT_FN(fn) [this](auto&&... a_args) -> decltype(auto) { return this->fn(std::forward<decltype(a_args)>(a_args)...); }

/// MYTodo:\
This event design requires that the event be passed through everything, this is probs bad \
a better way is too have a class that contains a listener and the callback function \
aka maybe change from dispatcher event sysytem to listener event system

namespace myl {
	//@brief user defined event types should not conflict with the engine event types below
	enum class event_type {
		none,

		window_close,
		window_resize,
		window_focus,
		window_lost_focus,
		window_moved,

		app_tick,
		app_update,
		app_render,

		key_pressed,
		key_released,
		key_typed,

		mouse_pressed,
		mouse_released,
		mouse_moved,
		mouse_scrolled
	};

	enum class event_category {
		none = 0,
		application = 1 << 0,
		input = 1 << 1,
		keyboard = 1 << 2,
		mouse = 1 << 3,
		mouse_button = 1 << 4
	};

#define MYL_IMPL_EVENT_CATEGORY(category) MYL_API constexpr virtual i32 category_flags() const override { return category; }
#define MYL_IMPL_EVENT_TYPE(type_)\
	MYL_API static constexpr event_type static_type() { return event_type::type_; }\
	MYL_API constexpr virtual event_type type() const override { return static_type(); }\
	MYL_API constexpr virtual const char* name() const override { return #type_; }

	class event {
	public:
		MYL_API constexpr virtual ~event() = default;

		MYL_API constexpr virtual event_type type() const = 0;
		MYL_API constexpr virtual const char* name() const = 0;
		MYL_API constexpr virtual i32 category_flags() const = 0;

		constexpr bool is_in_category(event_category a_category) { return category_flags() & static_cast<i32>(a_category); }

		bool handled = false;
	};

	class event_dispatcher {
		event& m_event;
	public:
		MYL_API constexpr event_dispatcher(event& a_event)
			: m_event(a_event) {}

		//@param a_func needs to return a bool
		template<typename T, typename Func>
		bool dispatch(const Func& a_func) {
			if (m_event.type() == T::static_type()) {
				m_event.handled |= a_func(static_cast<T&>(m_event));
				return true;
			}

			return false;
		}
	};
}
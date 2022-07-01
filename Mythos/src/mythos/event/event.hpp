#pragma once
#include <mythos/defines.hpp>

#include <functional>

/// MYTodo: I don't like this macro
#include <memory> /// For macro below
#define MYL_BIND_EVENT_FN(fn) [this](auto&&... a_args) -> decltype(auto) { return this->fn(std::forward<decltype(a_args)>(a_args)...); }

namespace myl::event {
	//@brief User defined event types should not conflict with the engine event types below
	enum class type {
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

	namespace category {
		enum {
			none = 0,
			application = 1 << 0,
			input = 1 << 1,
			keyboard = 1 << 2,
			mouse = 1 << 3,
			mouse_button = 1 << 4
		};
	}

#define MYL_IMPL_EVENT_CATEGORY(category_) MYL_API MYL_NO_DISCARD constexpr virtual myl::i32 category_flags() const override { return category_; }
#define MYL_IMPL_EVENT_TYPE(type_)\
	MYL_API MYL_NO_DISCARD static constexpr myl::event::type static_type() { return myl::event::type::type_; }\
	MYL_API MYL_NO_DISCARD constexpr virtual myl::event::type type() const override { return static_type(); }\
	MYL_API MYL_NO_DISCARD constexpr virtual const char* name() const override { return #type_; }

	class event_base {
	public:
		MYL_API constexpr virtual ~event_base() = default;

		MYL_API MYL_NO_DISCARD constexpr virtual type type() const = 0;
		MYL_API MYL_NO_DISCARD constexpr virtual const char* name() const = 0;
		MYL_API MYL_NO_DISCARD constexpr virtual i32 category_flags() const = 0;

		MYL_API MYL_NO_DISCARD constexpr bool is_in_category(i32 a_category) { return category_flags() & a_category; }

		bool handled = false;
	};

	class dispatcher {
		event_base& m_event;
	public:
		MYL_API constexpr dispatcher(event_base& a_event)
			: m_event(a_event) {}

		//@param a_func: Must return a bool
		template<typename T, typename Func>
		bool dispatch(const Func& a_func) {
			if (m_event.type() == T::static_type()) {
				m_event.handled |= a_func(static_cast<T&>(m_event));
				return true;
			}

			return false;
		}
	};

	using event_callback = std::function<void(event_base&)>;

	void set_event_callback(event_callback&);
	MYL_API void fire(event_base&);
}
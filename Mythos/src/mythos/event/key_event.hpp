#pragma once
#include "event.hpp"

#include <mythos/core/key_codes.hpp>

namespace myl {
	class key_event : public event {
	protected:
		key_code m_code;

		MYL_API constexpr key_event(const key_code a_code)
			: m_code(a_code) {}
	public:
		MYL_API MYL_NO_DISCARD constexpr key_code key() const { return m_code; }

		MYL_IMPL_EVENT_CATEGORY(static_cast<i32>(event_category::keyboard) | static_cast<i32>(event_category::input))
	};

	class event_key_pressed : public key_event {
		bool m_repeat;
	public:
		MYL_API constexpr event_key_pressed(const key_code a_code, bool a_repeat)
			: key_event(a_code)
			, m_repeat(a_repeat) {}

		MYL_API MYL_NO_DISCARD constexpr bool is_repeat() const { return m_repeat; }

		MYL_IMPL_EVENT_TYPE(key_pressed)
	};

	class event_key_released : public key_event {
	public:
		MYL_API event_key_released(const key_code a_code)
			: key_event(a_code) {}

		MYL_IMPL_EVENT_TYPE(key_released)
	};

	class event_key_typed : public key_event {
	public:
		MYL_API event_key_typed(const key_code a_code)
			: key_event(a_code) {}

		MYL_IMPL_EVENT_TYPE(key_typed)
	};
}
#pragma once
#include "event.hpp"

#include <mythos/core/key_codes.hpp>

namespace myl::event {
	class key_event_base : public event_base {
	protected:
		key_code m_code;

		MYL_API constexpr key_event_base(const key_code a_code)
			: m_code(a_code) {}
	public:
		MYL_API MYL_NO_DISCARD constexpr key_code key() const { return m_code; }

		MYL_IMPL_EVENT_CATEGORY(category::keyboard | category::input)
	};

	class key_pressed : public key_event_base {
		bool m_repeat;
	public:
		MYL_API constexpr key_pressed(const key_code a_code, bool a_repeat)
			: key_event_base(a_code)
			, m_repeat(a_repeat) {}

		MYL_API MYL_NO_DISCARD constexpr bool repeated() const { return m_repeat; }

		MYL_IMPL_EVENT_TYPE(key_pressed)
	};

	class key_released : public key_event_base {
	public:
		MYL_API key_released(const key_code a_code)
			: key_event_base(a_code) {}

		MYL_IMPL_EVENT_TYPE(key_released)
	};

	class key_typed : public key_event_base {
	public:
		MYL_API key_typed(const key_code a_code)
			: key_event_base(a_code) {}

		MYL_IMPL_EVENT_TYPE(key_typed)
	};
}
#include "event.hpp"

#include <mythos/core/log.hpp>

namespace myl {
	static event_callback* g_event_callback = nullptr;

	void set_event_callback(event_callback& a_callback) {
		g_event_callback = &a_callback;
	}

	void fire_event(event& a_event) {
		if (g_event_callback)
			(*g_event_callback)(a_event);
	}
}
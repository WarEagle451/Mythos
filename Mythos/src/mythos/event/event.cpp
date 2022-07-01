#include "event.hpp"

namespace myl::event {
	static event_callback* g_event_callback = nullptr;

	void set_event_callback(event_callback& a_callback) {
		g_event_callback = &a_callback;
	}

	void fire(event_base& a_event) {
		if (g_event_callback)
			(*g_event_callback)(a_event);
	}
}
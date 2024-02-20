#include <mythos/event/event.hpp>

namespace myth::event {
    static callback* g_callback = nullptr;

    auto set_callback(callback& c) -> void {
        g_callback = &c;
    }

    auto fire(base& e) -> void {
        if (g_callback)
            (*g_callback)(e);
    }
}

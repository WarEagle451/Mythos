#include <testbed/layers/event_layer.hpp>
#include <testbed/log.hpp>

namespace testbed {
    MYL_NO_DISCARD event_layer::event_layer()
        : myth::layer("Event Layer") {

    }

    event_layer::~event_layer() {

    }

    auto event_layer::on_event(myth::event::base& e) -> void {
        myth::event::dispatcher d(e);
        d.dispatch<myth::event::device_button_pressed>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_device_button_pressed));
        d.dispatch<myth::event::device_button_released>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_device_button_released));

        d.dispatch<myth::event::key_pressed>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_key_pressed));
        d.dispatch<myth::event::key_released>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_key_released));
        d.dispatch<myth::event::typed>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_typed));

        d.dispatch<myth::event::mouse_moved>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_mouse_moved));
        d.dispatch<myth::event::mouse_scrolled>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_mouse_scrolled));
        d.dispatch<myth::event::mouse_pressed>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_mouse_pressed));
        d.dispatch<myth::event::mouse_released>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_mouse_released));

        d.dispatch<myth::event::window_resize>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_window_resized));
        d.dispatch<myth::event::window_move>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_window_moved));
        d.dispatch<myth::event::window_close>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_window_closed));
        d.dispatch<myth::event::window_focus_gain>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_window_focus_gained));
        d.dispatch<myth::event::window_focus_lost>(MYTHOS_BIND_EVENT_FUNC(event_layer::on_window_focus_lost));
    }

    auto event_layer::on_device_button_pressed(myth::event::device_button_pressed& e) -> bool {
        TESTBED_TRACE("device button(s) pressed: {}", e.buttons());
        return false;
    }

    auto event_layer::on_device_button_released(myth::event::device_button_released& e) -> bool {
        TESTBED_TRACE("device button(s) released: {}", e.buttons());
        return false;
    }

    auto event_layer::on_key_pressed(myth::event::key_pressed& e) -> bool {
        TESTBED_TRACE("Key '{}' pressed, repeated: {}", e.key(), e.repeated());
        return false;
    }

    auto event_layer::on_key_released(myth::event::key_released& e) -> bool {
        TESTBED_TRACE("Key '{}' released", e.key());
        return false;
    }

    auto event_layer::on_typed(myth::event::typed& e) -> bool {
        TESTBED_TRACE("Character '{}' typed", e.character());
        return false;
    }

    auto event_layer::on_mouse_moved(myth::event::mouse_moved& e) -> bool {
        auto& position = e.position();
        TESTBED_TRACE("Mouse moved to x: {}, y: {}", position.x, position.y);
        return false;
    }

    auto event_layer::on_mouse_scrolled(myth::event::mouse_scrolled& e) -> bool {
        auto& scroll = e.delta();
        TESTBED_TRACE("Mouse scrolled  delta: [{}, {}]", scroll.x, scroll.y);
        return false;
    }

    auto event_layer::on_mouse_pressed(myth::event::mouse_pressed& e) -> bool {
        TESTBED_TRACE("Mouse button(s) pressed: {}", e.buttons());
        return false;
    }

    auto event_layer::on_mouse_released(myth::event::mouse_released& e) -> bool {
        TESTBED_TRACE("Mouse button(s) released: {}", e.buttons());
        return false;
    }

    auto event_layer::on_window_resized(myth::event::window_resize& e) -> bool {
        auto& dimensions = e.dim();
        TESTBED_TRACE("Window '{}' resized to w: {}, h: {}", e.window().title(), dimensions.w, dimensions.h);
        return false;
    }

    auto event_layer::on_window_moved(myth::event::window_move& e) -> bool {
        auto& position = e.position();
        TESTBED_TRACE("Window '{}' moved to x: {}, y: {}", e.window().title(), position.x, position.y);
        return false;
    }

    auto event_layer::on_window_closed(myth::event::window_close& e) -> bool {
        TESTBED_TRACE("Window '{}' closed", e.window().title());
        return false;
    }

    auto event_layer::on_window_focus_gained(myth::event::window_focus_gain& e) -> bool {
        TESTBED_TRACE("Window '{}' gained focus", e.window().title());
        return false;
    }

    auto event_layer::on_window_focus_lost(myth::event::window_focus_lost& e) -> bool {
        TESTBED_TRACE("Window '{}' lost focus", e.window().title());
        return false;
    }
}

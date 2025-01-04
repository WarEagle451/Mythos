#pragma once
#include <mythos/core/layer.hpp>
#include <mythos/event/device_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/event/window_event.hpp>

namespace testbed {
    class event_layer : public myth::layer {
    public:
        MYL_NO_DISCARD event_layer();
        ~event_layer() override;

        auto on_attach() -> void override {}
        auto on_detach() -> void override {}
        auto on_event(myth::event::base& e) -> void override;
        auto update(MYL_MAYBE_UNUSED myth::timestep ts) -> void override {}
        auto render() -> void override {}
    private:
        auto on_device_button_pressed(myth::event::device_button_pressed&) -> bool;
        auto on_device_button_released(myth::event::device_button_released&) -> bool;

        auto on_key_pressed(myth::event::key_pressed&) -> bool;
        auto on_key_released(myth::event::key_released&) -> bool;
        auto on_typed(myth::event::typed&) -> bool;

        auto on_mouse_moved(myth::event::mouse_moved&) -> bool;
        auto on_mouse_scrolled(myth::event::mouse_scrolled&) -> bool;
        auto on_mouse_pressed(myth::event::mouse_pressed&) -> bool;
        auto on_mouse_released(myth::event::mouse_released&) -> bool;

        auto on_window_resized(myth::event::window_resize&) -> bool;
        auto on_window_moved(myth::event::window_move&) -> bool;
        auto on_window_closed(myth::event::window_close&) -> bool;
        auto on_window_focus_gained(myth::event::window_focus_gain&) -> bool;
        auto on_window_focus_lost(myth::event::window_focus_lost&) -> bool;
    };
}

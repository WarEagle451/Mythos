#pragma once
#include <mythos/core/layer.hpp>
#include <mythos/event/device_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/event/window_event.hpp>

namespace testbed {
    class event_layer : public mye::layer {
    public:
        MYL_NO_DISCARD event_layer();
        ~event_layer() override;

        auto on_attach() -> void override {}
        auto on_detach() -> void override {}
        auto on_event(mye::event::base& e) -> void override;
        auto update(MYL_MAYBE_UNUSED mye::timestep ts) -> void override {}
        auto render() -> void override {}
    private:
        auto on_device_button_pressed(mye::event::device_button_pressed&) -> bool;
        auto on_device_button_released(mye::event::device_button_released&) -> bool;

        auto on_key_pressed(mye::event::key_pressed&) -> bool;
        auto on_key_released(mye::event::key_released&) -> bool;
        auto on_typed(mye::event::typed&) -> bool;

        auto on_mouse_moved(mye::event::mouse_moved&) -> bool;
        auto on_mouse_scrolled(mye::event::mouse_scrolled&) -> bool;
        auto on_mouse_pressed(mye::event::mouse_pressed&) -> bool;
        auto on_mouse_released(mye::event::mouse_released&) -> bool;

        auto on_window_resized(mye::event::window_resize&) -> bool;
        auto on_window_moved(mye::event::window_move&) -> bool;
        auto on_window_closed(mye::event::window_close&) -> bool;
        auto on_window_focus_gained(mye::event::window_focus_gain&) -> bool;
        auto on_window_focus_lost(mye::event::window_focus_lost&) -> bool;
    };
}

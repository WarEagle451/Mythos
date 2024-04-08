#pragma once
#include <mythos/core/layer.hpp>
#include <mythos/event/gamepad_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>

namespace testbed {
    class testbed_layer final : public myth::layer {
    public:
        MYL_NO_DISCARD testbed_layer();
        ~testbed_layer() override;

        auto on_attach() -> void override;
        auto on_detach() -> void override;
        auto on_event(myth::event::base& e) -> void override;
        auto update(MYL_MAYBE_UNUSED myth::timestep ts) -> void override;
        auto render() -> void override;
    private:
        auto on_key_pressed(myth::event::key_pressed& e) -> bool;
        auto on_key_released(myth::event::key_released& e) -> bool;
        auto on_typed(myth::event::typed& e) -> bool;
        auto on_mouse_moved(myth::event::mouse_moved& e) -> bool;
        auto on_mouse_scrolled(myth::event::mouse_scrolled& e) -> bool;
        auto on_mouse_pressed(myth::event::mouse_pressed& e) -> bool;
        auto on_mouse_released(myth::event::mouse_released& e) -> bool;
        auto on_gamepad_button_pressed(myth::event::gamepad_button_pressed& e) -> bool;
        auto on_gamepad_button_released(myth::event::gamepad_button_released& e) -> bool;
    };
}

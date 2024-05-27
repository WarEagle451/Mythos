#pragma once
#include <mythos/core/layer.hpp>
#include <mythos/event/gamepad_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>

namespace testbed {
    class testbed_layer final : public myth::layer {
        myth::timestep m_smooth_ts = 0.f;
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
    };
}

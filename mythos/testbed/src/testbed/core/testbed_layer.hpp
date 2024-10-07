#pragma once
#include <mythos/core/layer.hpp>
#include <mythos/event/hid_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/event/window_event.hpp>

#include <testbed/camera.hpp>

namespace testbed {
    class testbed_layer final : public myth::layer {
        myth::timestep m_smooth_ts = 0.f;
        camera m_camera;
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
        auto on_window_resized(myth::event::window_resize& e) -> bool;
    };
}

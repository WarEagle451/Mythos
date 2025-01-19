#pragma once
#include <mythos/core/layer.hpp>
#include <mythos/event/device_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/event/window_event.hpp>

#include <testbed/camera.hpp>

namespace testbed {
    class testbed_layer final : public mye::layer {
        mye::timestep m_smooth_ts = 0.f;
        camera m_camera;
    public:
        MYL_NO_DISCARD testbed_layer();
        ~testbed_layer() override;

        auto on_attach() -> void override;
        auto on_detach() -> void override;
        auto on_event(mye::event::base& e) -> void override;
        auto update(MYL_MAYBE_UNUSED mye::timestep ts) -> void override;
        auto render() -> void override;
    private:
        auto on_key_pressed(mye::event::key_pressed& e) -> bool;
        auto on_window_resized(mye::event::window_resize& e) -> bool;
    };
}

#include <testbed/core/testbed_layer.hpp>
#include <testbed/log.hpp>

#include <mythos/core/application.hpp>
#include <mythos/input.hpp>
#include <mythos/render/renderer.hpp>

#include <format>

namespace testbed {
    MYL_NO_DISCARD testbed_layer::testbed_layer()
        : myth::layer("testbed") {
            
    }

    testbed_layer::~testbed_layer() {
        
    }

    auto testbed_layer::on_attach() -> void {
    
    }

    auto testbed_layer::on_detach() -> void {

    }

    auto testbed_layer::on_event(myth::event::base& e) -> void {
        myth::event::dispatcher d(e);
        d.dispatch<myth::event::key_pressed>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_key_pressed));
    }

    auto testbed_layer::update(MYL_MAYBE_UNUSED myth::timestep ts) -> void {
        if (myth::application::get().main_window()->state() != myth::window_state::minimized) {
            m_smooth_ts = (m_smooth_ts * 0.995f) + (ts * 0.005f); // Update timestep smoothly = (old_timestep * smoothing_factor) + (new_timestep * (1.0 - smoothing_factor))
            myth::application::get().main_window()->set_title(std::format("FPS: {:.2f}", 1.f / m_smooth_ts).c_str());
        }
    }

    auto testbed_layer::render() -> void {
        /// MYTEMP: Replace with a proper draw command
        myth::draw();
    }

    auto testbed_layer::on_key_pressed(myth::event::key_pressed& e) -> bool {
        TESTBED_TRACE("Key '{}' {}", e.key(), e.repeated() ? "held" : "pressed");

        switch(e.key()) {
            case myth::key::f1: {
                auto& app = myth::application::get();
                app.cursor_captured() ?
                    app.release_cursor() :
                    app.capture_cursor(app.main_window(), !myth::input::key_down(myth::key::left_shift));
                return true;
            }
            case myth::key::f2:
                myth::application::get().main_window()->set_position({ 100, 100 });
                return true;
            case myth::key::f3: {
                auto& window = *myth::application::get().main_window();
                window.set_style(window.style() == myth::window_style::fullscreen ? myth::window_style::windowed : myth::window_style::fullscreen);
                return true;
            }
            case myth::key::f4:
                if (myth::input::key_down(myth::key::left_alt))
                    myth::application::get().quit();
                else
                    myth::application::get().main_window()->set_dimensions({ 400, 400 });
                return true;
           
            case myth::key::f5: {
                myth::application::get().main_window()->restore();
                return true;
            }
        }

        return false;
    }
}

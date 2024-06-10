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

    myl::f32vec3 clear_color{ 0.f, 0.f, 0.f };
    myl::u32 clear_color_index = 0;
    myl::vec3<bool> clear_color_count_up{ true, true, true };

    bool vsync_toggle = true;

    auto testbed_layer::update(MYL_MAYBE_UNUSED myth::timestep ts) -> void {
        if (myth::application::get().main_window()->state() != myth::window_state::minimized) {
            ///m_smooth_ts = (m_smooth_ts * 0.995f) + (ts * 0.005f); // Update timestep smoothly = (old_timestep * smoothing_factor) + (new_timestep * (1.0 - smoothing_factor))
            ///myth::application::get().main_window()->set_title(std::format("FPS: {:.2f}", 1.f / m_smooth_ts).c_str());
            myth::application::get().main_window()->set_title(std::format("FPS: {:.2f}", 1.f / static_cast<float>(ts)).c_str());
        }

        if (clear_color_count_up[clear_color_index]) { /// There must be a way to make this smoother
            clear_color[clear_color_index] += 0.005f;
            if (clear_color[clear_color_index] >= 1.f) {
                clear_color_count_up[clear_color_index] = !clear_color_count_up[clear_color_index];
                ++clear_color_index;
                if (clear_color_index == 3)
                    clear_color_index = 0;
            }
        }
        else {
            clear_color[clear_color_index] -= 0.005f;
            if (clear_color[clear_color_index] <= 0.f) {
                clear_color_count_up[clear_color_index] = !clear_color_count_up[clear_color_index];
                ++clear_color_index;
                if (clear_color_index == 3)
                    clear_color_index = 0;
            }
        }

        myth::renderer::set_clear_color(clear_color);
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
            case myth::key::f6: {
                myth::renderer::set_vsync(vsync_toggle);
                vsync_toggle = !vsync_toggle;
            }
        }

        return false;
    }
}

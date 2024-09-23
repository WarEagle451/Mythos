#include <testbed/core/testbed_layer.hpp>
#include <testbed/log.hpp>

#include <mythos/core/application.hpp>
#include <mythos/input.hpp>
#include <mythos/render/renderer.hpp>

#include <myl/math/projection.hpp>
#include <myl/math/transform.hpp>
#include <myl/math/trigonometry.hpp>

#include <format>

#include <myl_wip/color.hpp>

namespace testbed {
    MYL_NO_DISCARD testbed_layer::testbed_layer()
        : myth::layer("testbed")
        , m_camera({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, }, { 1.f, 1.f, 1.f }, 0.f) {
        m_camera.on_window_resize(myth::application::get().main_window()->dimensions());
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
        d.dispatch<myth::event::window_resize>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_window_resized));
    }

    myl::f32vec3 hsv_color{ 0.f, 1.f, 1.f };
    bool vsync_toggle = true;

    auto testbed_layer::update(MYL_MAYBE_UNUSED myth::timestep ts) -> void {
        if (myth::application::get().main_window()->state() != myth::window_state::minimized)
            myth::application::get().main_window()->set_title(std::format("FPS: {:.2f}", 1.f / static_cast<float>(ts)).c_str());

        m_camera.on_update(ts);

        hsv_color[0] += 100.f * ts;
        if (hsv_color[0] >= 360.f)
            hsv_color[0] -= 360.f;
    }

    auto testbed_layer::render() -> void {
        myth::renderer::begin(m_camera.view_projection());

        myth::renderer::draw_quad({ 0.f, 0.f, 5.f }, { 1, 1, 1, 1 });
        myth::renderer::draw_quad({ -.5f, .5f, 5.f }, { 1, 0, 0, 1 });
        myth::renderer::draw_quad({ .5f, .5f, 5.f }, { 0, 1, 0, 1 });
        myth::renderer::draw_quad({ .5f, -.5f, 5.f }, { 0, 0, 1, 1 });
        myth::renderer::draw_quad({ -.5f, -.5f, 5.f }, { 1, 1, 0, 1 });
        myth::renderer::flush_batch();
        myth::renderer::begin_batch();
        myth::renderer::draw_quad({ 0, 0, 5.f }, myl::hsv_to_rgba(hsv_color));

        myth::renderer::end();
    }

    auto testbed_layer::on_key_pressed(myth::event::key_pressed& e) -> bool {
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

    auto testbed_layer::on_window_resized(myth::event::window_resize& e) -> bool {
        m_camera.on_window_resize(e.dim());
        return true;
    }
}

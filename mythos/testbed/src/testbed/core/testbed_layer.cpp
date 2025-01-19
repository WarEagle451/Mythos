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
        : mye::layer("testbed")
        , m_camera({ 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f, }, { 1.f, 1.f, 1.f }, 0.f) {
        m_camera.on_window_resize(mye::application::get().main_window()->dimensions());
    }

    testbed_layer::~testbed_layer() {
        
    }

    auto testbed_layer::on_attach() -> void {
        
    }

    auto testbed_layer::on_detach() -> void {

    }

    auto testbed_layer::on_event(mye::event::base& e) -> void {
        mye::event::dispatcher d(e);
        d.dispatch<mye::event::key_pressed>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_key_pressed));
        d.dispatch<mye::event::window_resize>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_window_resized));
    }

    myl::f32vec3 hsv_color{ 0.f, 1.f, 1.f };
    bool vsync_toggle = true;

    auto testbed_layer::update(MYL_MAYBE_UNUSED mye::timestep ts) -> void {
        m_camera.on_update(ts);

        hsv_color[0] += 100.f * ts;
        if (hsv_color[0] >= 360.f)
            hsv_color[0] -= 360.f;
    }

    auto testbed_layer::render() -> void {
        mye::renderer::begin(m_camera.view_projection());

        mye::renderer::draw_quad({ 0.f, 0.f, 5.f }, { 1, 1, 1, 1 });
        mye::renderer::draw_quad({ -.5f, .5f, 5.f }, { 1, 0, 0, 1 });
        mye::renderer::draw_quad({ .5f, .5f, 5.f }, { 0, 1, 0, 1 });
        mye::renderer::draw_quad({ .5f, -.5f, 5.f }, { 0, 0, 1, 1 });
        mye::renderer::draw_quad({ -.5f, -.5f, 5.f }, { 1, 1, 0, 1 });
        mye::renderer::flush_batch();
        mye::renderer::begin_batch();
        mye::renderer::draw_quad({ 0, 0, 5.f }, myl::hsv_to_rgba(hsv_color));

        mye::renderer::end();
    }

    auto testbed_layer::on_key_pressed(mye::event::key_pressed& e) -> bool {
        switch(e.key()) {
            case mye::key::f1: {
                auto& app = mye::application::get();
                app.cursor_captured() ?
                    app.release_cursor() :
                    app.capture_cursor(app.main_window(), !mye::input::key_down(mye::key::left_shift));
                return true;
            }
            case mye::key::f2:
                mye::application::get().main_window()->set_position({ 100, 100 });
                return true;
            case mye::key::f3: {
                auto& window = *mye::application::get().main_window();
                window.set_style(window.style() == mye::window_style::fullscreen ? mye::window_style::windowed : mye::window_style::fullscreen);
                return true;
            }
            case mye::key::f4:
                if (mye::input::key_down(mye::key::left_alt))
                    mye::application::get().quit();
                else
                    mye::application::get().main_window()->set_dimensions({ 400, 400 });
                return true;
           
            case mye::key::f5: {
                mye::application::get().main_window()->restore();
                return true;
            }
            case mye::key::f6: {
                mye::renderer::set_vsync(vsync_toggle);
                vsync_toggle = !vsync_toggle;
            }
        }

        return false;
    }

    auto testbed_layer::on_window_resized(mye::event::window_resize& e) -> bool {
        m_camera.on_window_resize(e.dim());
        return true;
    }
}

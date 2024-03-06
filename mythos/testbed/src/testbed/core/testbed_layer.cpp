#include <testbed/core/testbed_layer.hpp>
#include <testbed/log.hpp>

#include <mythos/core/application.hpp>
#include <mythos/input.hpp>

#include <Windows.h>

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
        d.dispatch<myth::event::key_released>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_key_released));
        d.dispatch<myth::event::typed>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_typed));
        d.dispatch<myth::event::mouse_moved>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_mouse_moved));
        d.dispatch<myth::event::mouse_scrolled>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_mouse_scrolled));
        d.dispatch<myth::event::mouse_pressed>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_mouse_pressed));
        d.dispatch<myth::event::mouse_released>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_mouse_released));
        d.dispatch<myth::event::gamepad_button_pressed>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_gamepad_button_pressed));
        d.dispatch<myth::event::gamepad_button_released>(MYTHOS_BIND_EVENT_FUNC(testbed_layer::on_gamepad_button_released));
    }

    auto testbed_layer::update(myth::timestep ts) -> void {
        auto ls = myth::input::left_stick_delta(); 
        if (ls.x != 0.f || ls.y != 0.f) {
            ls = myl::normalize(ls);
            ls += 1.f;
            ls *= .5f;

            POINT p{};
            GetCursorPos(&p);
            p.x += static_cast<LONG>(std::lerp(-3.f, 3.f, ls.x));
            p.y += static_cast<LONG>(std::lerp(-3.f, 3.f, ls.y));
            SetCursorPos(p.x, p.y);
        }
    }

    auto testbed_layer::render() -> void {
        
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

    auto testbed_layer::on_key_released(myth::event::key_released& e) -> bool {
        TESTBED_TRACE("Key '{}' released", e.key());
        return true;
    }

    auto testbed_layer::on_typed(myth::event::typed& e) -> bool {
        TESTBED_TRACE("Typed '{}'", e.character());
        return true;
    }

    auto testbed_layer::on_mouse_moved(myth::event::mouse_moved& e) -> bool {
        TESTBED_TRACE("Mouse moved [{}, {}]", e.position().x, e.position().y);
        return true;
    }

    auto testbed_layer::on_mouse_scrolled(myth::event::mouse_scrolled& e) -> bool {
        TESTBED_TRACE("Mouse scrolled [{}, {}]", e.delta().x, e.delta().y);
        return true;
    }

    auto testbed_layer::on_mouse_pressed(myth::event::mouse_pressed& e) -> bool {
        TESTBED_TRACE("Mouse button(s) '{}' pressed", e.buttons());
        return true;
    }

    auto testbed_layer::on_mouse_released(myth::event::mouse_released& e) -> bool {
        TESTBED_TRACE("Mouse button(s) '{}' released", e.buttons());
        return true;
    }

    auto testbed_layer::on_gamepad_button_pressed(myth::event::gamepad_button_pressed& e) -> bool {
        MYTHOS_ERROR("Gamepad button(s): {} pressed", e.buttons());
        if (e.buttons() & myth::button::ps_cross) {
            constexpr int count = 2;
            INPUT click[count]{
                {
                    .type = INPUT_MOUSE,
                    .mi = {
                        .dx = 0,
                        .dy = 0,
                        .mouseData = XBUTTON1,
                        .dwFlags = MOUSEEVENTF_LEFTDOWN
                    }
                },
                {
                    .type = INPUT_MOUSE,
                    .mi = {
                        .dx = 0,
                        .dy = 0,
                        .mouseData = XBUTTON1,
                        .dwFlags = MOUSEEVENTF_LEFTUP
                    }
                }
            };

            SendInput(count, click, sizeof(INPUT));
        }

        return true;
    }

    auto testbed_layer::on_gamepad_button_released(myth::event::gamepad_button_released& e) -> bool {
        MYTHOS_ERROR("Gamepad button(s): {} released", e.buttons());
        return true;
    }
}

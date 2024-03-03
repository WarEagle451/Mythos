#include <editor/core/editor_layer.hpp>
#include <editor/log.hpp>

#include <mythos/core/application.hpp>
#include <mythos/input.hpp>

namespace editor {
    MYL_NO_DISCARD editor_layer::editor_layer()
        : myth::layer("Editor") {
            
    }

    editor_layer::~editor_layer() {
        
    }

    auto editor_layer::on_attach() -> void {
    
    }

    auto editor_layer::on_detach() -> void {

    }

    auto editor_layer::on_event(myth::event::base& e) -> void {
        myth::event::dispatcher d(e);
        d.dispatch<myth::event::key_pressed>(MYTHOS_BIND_EVENT_FUNC(editor_layer::on_key_pressed));
        d.dispatch<myth::event::key_released>(MYTHOS_BIND_EVENT_FUNC(editor_layer::on_key_released));
        d.dispatch<myth::event::typed>(MYTHOS_BIND_EVENT_FUNC(editor_layer::on_typed));
        d.dispatch<myth::event::mouse_moved>(MYTHOS_BIND_EVENT_FUNC(editor_layer::on_mouse_moved));
        d.dispatch<myth::event::mouse_scrolled>(MYTHOS_BIND_EVENT_FUNC(editor_layer::on_mouse_scrolled));
        d.dispatch<myth::event::mouse_pressed>(MYTHOS_BIND_EVENT_FUNC(editor_layer::on_mouse_pressed));
        d.dispatch<myth::event::mouse_released>(MYTHOS_BIND_EVENT_FUNC(editor_layer::on_mouse_released));
    }

    auto editor_layer::update(myth::timestep ts) -> void {
        
    }

    auto editor_layer::render() -> void {
        
    }

    auto editor_layer::on_key_pressed(myth::event::key_pressed& e) -> bool {
        EDITOR_TRACE("Key '{}' {}", e.key(), e.repeated() ? "held" : "pressed");

        switch(e.key()) {
            case myth::key::f1:
                myth::application::get().capture_cursor(myth::application::get().main_window(), true);
                return true;
            case myth::key::f2:
                myth::application::get().release_cursor();
                return true;
            case myth::key::f3:
                myth::application::get().main_window()->set_position({ 100, 100 });
                return true;
            case myth::key::f4:
                if (myth::input::key_down(myth::key::left_alt))
                    myth::application::get().quit();
                else
                    myth::application::get().main_window()->set_dimensions({ 400, 400 });
                return true;
        }

        return false;
    }

    auto editor_layer::on_key_released(myth::event::key_released& e) -> bool {
        EDITOR_TRACE("Key '{}' released", e.key());
        return true;
    }

    auto editor_layer::on_typed(myth::event::typed& e) -> bool {
        EDITOR_TRACE("Typed '{}'", e.character());
        return true;
    }

    auto editor_layer::on_mouse_moved(myth::event::mouse_moved& e) -> bool {
        EDITOR_TRACE("Mouse moved [{}, {}]", e.position().x, e.position().y);
        return true;
    }

    auto editor_layer::on_mouse_scrolled(myth::event::mouse_scrolled& e) -> bool {
        EDITOR_TRACE("Mouse scrolled [{}, {}]", e.delta().x, e.delta().y);
        return true;
    }

    auto editor_layer::on_mouse_pressed(myth::event::mouse_pressed& e) -> bool {
        EDITOR_TRACE("Mouse button(s) '{}' pressed", e.buttons());
        return true;
    }

    auto editor_layer::on_mouse_released(myth::event::mouse_released& e) -> bool {
        EDITOR_TRACE("Mouse button(s) '{}' released", e.buttons());
        return true;
    }
}

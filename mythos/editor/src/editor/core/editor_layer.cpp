#include <editor/core/editor_layer.hpp>
#include <editor/log.hpp>

#include <mythos/core/application.hpp>

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
    }

    auto editor_layer::update(myth::timestep ts) -> void {

    }

	auto editor_layer::render() -> void {
        
    }

    auto editor_layer::on_key_pressed(myth::event::key_pressed& e) -> bool {
        EDITOR_TRACE("Key '{}' {}", e.key(), e.repeated() ? "held" : "pressed");
        return true;
    }

    auto editor_layer::on_key_released(myth::event::key_released& e) -> bool {
        EDITOR_TRACE("Key '{}' released", e.key());
        return true;
    }

    auto editor_layer::on_typed(myth::event::typed& e) -> bool {
        EDITOR_TRACE("Typed '{}'", e.character());
        return true;
    }
}

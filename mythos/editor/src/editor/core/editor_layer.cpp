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
        
    }

    auto editor_layer::update(myth::timestep ts) -> void {

    }

	auto editor_layer::render() -> void {
        
    }
}

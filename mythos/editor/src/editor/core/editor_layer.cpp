#include <editor/core/editor_layer.hpp>
#include <editor/log.hpp>

namespace editor {
    MYL_NO_DISCARD editor_layer::editor_layer()
        : myth::layer("Editor") {
        EDITOR_TRACE("Trace");
        EDITOR_DEBUG("Debug");
        EDITOR_INFO("Info");
        EDITOR_WARN("Warning");
        EDITOR_ERROR("Error");
        EDITOR_FATAL("Fatal");
    }

    editor_layer::~editor_layer() {
        
    }

    auto editor_layer::on_attach() -> void {

    }

	auto editor_layer::on_detach() -> void {

    }

    auto editor_layer::update() -> void {

    }

	auto editor_layer::render() -> void {
        
    }
}

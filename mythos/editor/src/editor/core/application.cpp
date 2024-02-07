#include <editor/core/application.hpp>
#include <editor/core/editor_layer.hpp>
#include <editor/log.hpp>

#include <mythos/entry.hpp>

#include <memory>

namespace editor {
    application::application()
        : myth::application() {
        log::init();
        push_layer(std::make_unique<editor_layer>());
    }

    application::~application() {

    }
}

namespace myth {
    auto create_application() -> std::unique_ptr<application> {
        auto app = std::make_unique<editor::application>();
        return app;
    }
}

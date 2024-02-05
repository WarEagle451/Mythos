#include <editor/application.hpp>

namespace editor {
    application::application()
    : myth::application() {

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

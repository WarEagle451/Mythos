#include <editor/core/application.hpp>
#include <editor/core/editor_layer.hpp>
#include <editor/log.hpp>

#include <mythos/entry.hpp>
#include <mythos/version.hpp>

#include <memory>

namespace editor {
    application::application(const myth::application_specification& specs)
        : myth::application(specs) {
        log::init();
        push_layer(std::make_unique<editor_layer>());
    }

    application::~application() {

    }
}

namespace myth {
    auto create_application() -> std::unique_ptr<application> {
        myth::application_specification specs{
            .info{
               .name = "Mythos Editor",
               .version_major = MYTHOS_VERSION_MAJOR,
               .version_minor = MYTHOS_VERSION_MINOR,
               .version_patch = MYTHOS_VERSION_PATCH,
            }
        };

        auto app = std::make_unique<editor::application>(specs);
        return app;
    }
}

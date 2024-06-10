#include <testbed/core/application.hpp>
#include <testbed/core/testbed_layer.hpp>
#include <testbed/log.hpp>

#include <mythos/entry.hpp>
#include <mythos/version.hpp>

#include <memory>

namespace testbed {
    application::application(const myth::application_specification& specs)
        : myth::application(specs) {
        log::init();
        push_layer(std::make_unique<testbed_layer>());
    }

    application::~application() {

    }
}

namespace myth {
    auto create_application() -> std::unique_ptr<application> {
        myth::application_specification specs{
            .info{
               .name = "Mythos Testbed",
               .version_major = MYTHOS_VERSION_MAJOR,
               .version_minor = MYTHOS_VERSION_MINOR,
               .version_patch = MYTHOS_VERSION_PATCH,
            },
            .window_config{
                .title = "Mythos Testbed",
                .center_cursor = true
            },
            .renderer_config{
                .clear_color{ .2f, .2f, .2f },
                .vsync = true
            },
            .enable_trace_logging = true
        };

        auto app = std::make_unique<testbed::application>(specs);
        return app;
    }
}

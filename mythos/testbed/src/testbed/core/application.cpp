#include <testbed/core/application.hpp>
#include <testbed/core/testbed_layer.hpp>
#include <testbed/layers/event_layer.hpp>
#include <testbed/log.hpp>

#include <mythos/entry.hpp>
#include <mythos/version.hpp>

#include <memory>

namespace testbed {
    application::application(const mye::application_specification& specs)
        : mye::application(specs) {
        log::init();
        //push_layer(std::make_unique<event_layer>());
        push_layer(std::make_unique<testbed_layer>());
    }

    application::~application() {

    }
}

namespace mye {
    auto create_application() -> std::unique_ptr<application> {
        mye::application_specification specs{
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
                .clear_color{ .0f, .0f, .0f },
                .vsync = true
            },
            .enable_trace_logging = true
        };

        auto app = std::make_unique<testbed::application>(specs);
        return app;
    }
}

#pragma once
#include <myl/definitions.hpp>

#include <memory>

namespace myth {
    class application {
    public:
        auto run() -> void;
    };

    // To be defined in client
    MYL_NO_DISCARD auto create_application() -> std::unique_ptr<application>;
}

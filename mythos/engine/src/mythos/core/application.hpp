#pragma once
#include <myl/definitions.hpp>
#include <myl/memory.hpp>

namespace myth {
    class application {
        static myl::observer_ptr<application> s_instance;
    public:
        MYL_API auto set(const myl::observer_ptr<application>& ptr) -> void;
        MYL_NO_DISCARD MYL_API auto get() -> application&;

        MYL_NO_DISCARD application();
        ~application();

        auto run() -> void;
    };

    // To be defined in client
    MYL_NO_DISCARD auto create_application() -> std::unique_ptr<application>;
}

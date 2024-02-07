#pragma once
#include <mythos/core/layer_stack.hpp>

#include <myl/definitions.hpp>
#include <myl/memory.hpp>

namespace myth {
    class application {
        static myl::observer_ptr<application> s_instance;

        layer_stack m_layer_stack;

        bool m_running;
        bool m_suspended;
    public:
        MYL_NO_DISCARD MYL_API constexpr auto get() -> application&;

        MYL_NO_DISCARD application();
        application(const application&) = delete;
        virtual ~application();

        auto push_layer(std::unique_ptr<layer>&& l) -> void;
        auto push_overlay(std::unique_ptr<layer>&& o) -> void;

        auto run() -> void;
        auto quit() noexcept -> void;

        auto operator=(const application&) -> application& = delete;
    };

    // To be defined in client
    extern auto create_application() -> std::unique_ptr<application>;
}

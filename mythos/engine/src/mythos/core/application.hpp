#pragma once
#include <mythos/core/layer_stack.hpp>
#include <mythos/platform/window.hpp>

#include <myl/definitions.hpp>
#include <myl/memory.hpp>

namespace myth {
    struct application_information {
        const char* name = "Mythos Application";
        myl::u16 version_major = 0;
        myl::u16 version_minor = 0;
        myl::u16 version_patch = 0;
    };

    struct application_specification {
        application_information info{};
    };

    struct application_statistics {
        timestep timestep = 0.f;
    };

    class application {
        static myl::observer_ptr<application> s_instance;

        application_information m_info;
        application_statistics m_stats;

        layer_stack m_layer_stack;
        std::unique_ptr<window> m_window;

        bool m_running;
        bool m_suspended;
    public:
        MYL_NO_DISCARD static MYL_API constexpr auto get() -> application&;

        MYL_NO_DISCARD application(const application_specification& specs);
        application(const application&) = delete;
        virtual ~application();

        MYL_NO_DISCARD constexpr auto info() const noexcept -> const application_information& { return m_info; }
        MYL_NO_DISCARD constexpr auto stats() const noexcept -> const application_statistics& { return m_stats; }
        MYL_NO_DISCARD constexpr auto main_window() noexcept -> window* { return m_window.get(); }

        auto push_layer(std::unique_ptr<layer>&& l) -> void;
        auto push_overlay(std::unique_ptr<layer>&& o) -> void;

        auto run() -> void;
        auto quit() noexcept -> void;

        auto operator=(const application&) -> application& = delete;
    };

    // To be defined in client
    extern auto create_application() -> std::unique_ptr<application>;
}

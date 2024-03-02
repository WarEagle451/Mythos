#pragma once
#include <mythos/core/timestep.hpp>
#include <mythos/event/event.hpp>

#include <myl/definitions.hpp>

#include <string>
#include <string_view>

namespace myth {
    class layer {
    protected:
        std::string m_name;
    public:
        MYL_NO_DISCARD constexpr layer(std::string_view name) noexcept
            : m_name{ name } {}

        virtual ~layer() = default;

        virtual auto on_attach() -> void = 0;
        virtual auto on_detach() -> void = 0;
        virtual auto on_event(event::base& e) -> void = 0;
        virtual auto update(timestep ts) -> void = 0;
        virtual auto render() -> void = 0;

        MYL_NO_DISCARD constexpr auto name() const noexcept -> std::string_view { return m_name; }
    };
}

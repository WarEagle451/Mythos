#pragma once
#include <myl/definitions.hpp>
#include <myl/math/vec2.hpp>

#include <memory>

namespace myth {
    enum class window_state {
        unknown,
        normal,
        maximized,
        minimized
    };

    struct window_configuration {
        const char* title = "Mythos Application";

        myl::i32vec2 dimensions = { 800, 600 };
        myl::i32vec2 position = { -1, -1 };
        window_state state = window_state::normal;
    };

    class window {
    protected:
        const char* m_title;

        myl::i32vec2 m_dimensions;
        myl::i32vec2 m_position;
        window_state m_state;
    public:
        MYL_NO_DISCARD static auto create(const window_configuration& config = window_configuration{}) -> std::unique_ptr<window>;

        MYL_NO_DISCARD constexpr window(const window_configuration& config)
            : m_title{ config.title }
            , m_dimensions{ config.dimensions }
            , m_position{ config.position }
            , m_state{ config.state } {}

        virtual ~window() = default;

        MYL_NO_DISCARD constexpr auto title() const noexcept -> const char* { return m_title; }
        MYL_NO_DISCARD constexpr auto state() const noexcept -> window_state { return m_state; }
        MYL_NO_DISCARD constexpr auto position() const noexcept -> const myl::i32vec2& { return m_position; }
        MYL_NO_DISCARD constexpr auto dimensions() const noexcept -> const myl::i32vec2& { return m_dimensions; }

        virtual auto set_title(const char*) -> void = 0;
        virtual auto set_state(window_state) -> void = 0;
        virtual auto set_position(const myl::i32vec2&) -> void = 0;
        virtual auto set_dimensions(const myl::i32vec2&) -> void = 0;

        virtual auto update() -> void = 0;
    };
}

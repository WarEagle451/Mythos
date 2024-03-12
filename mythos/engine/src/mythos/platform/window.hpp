#pragma once
#include <myl/definitions.hpp>
#include <myl/math/vec2.hpp>

#include <memory>

/// MYTODO: Window configuration
/// - Load window icon
/// - Window can be created on different monitors
/// - Multiple windows

namespace myth {
    enum class window_style {
        windowed,
        fullscreen,
        popup,
        unknown
    };

    enum class window_state {
        normal,
        maximized,
        minimized,
        unknown
    };

    struct window_configuration {
        const char* title = "Mythos Application Window";
        //const char* icon = "mythos/resources/branding/logo.png";

        myl::i32vec2 dimensions = { 800, 600 };
        myl::i32vec2 position = myl::i32vec2(-1);
        window_style style = window_style::windowed;
        window_state state = window_state::normal;

        bool center_cursor = true;
    };

    class window {
    protected:
        const char* m_title;

        myl::i32vec2 m_dimensions;
        myl::i32vec2 m_position;
        window_style m_style;
        window_state m_state;
    public:
        MYL_NO_DISCARD static MYL_API auto create(const window_configuration& config = window_configuration{}) -> std::unique_ptr<window>;

        MYL_NO_DISCARD constexpr window(const window_configuration& config)
            : m_title{ config.title }
            , m_dimensions{ config.dimensions }
            , m_position{ config.position }
            , m_style{ config.style }
            , m_state{ config.state } {}

        virtual ~window() = default;

        MYL_NO_DISCARD constexpr auto title() const noexcept -> const char* { return m_title; }
        MYL_NO_DISCARD constexpr auto style() const noexcept -> window_style { return m_style; }
        MYL_NO_DISCARD constexpr auto state() const noexcept -> window_state { return m_state; }
        MYL_NO_DISCARD constexpr auto position() const noexcept -> const myl::i32vec2& { return m_position; }
        MYL_NO_DISCARD constexpr auto dimensions() const noexcept -> const myl::i32vec2& { return m_dimensions; }

        virtual auto set_title(const char*) -> void = 0;
        virtual auto set_style(window_style) -> void = 0;
        virtual auto set_state(window_state) -> void = 0;
        virtual auto set_position(const myl::i32vec2&) -> void = 0;
        virtual auto set_dimensions(const myl::i32vec2&) -> void = 0;

        virtual auto close() -> void = 0;
        virtual auto restore() -> void = 0;
        virtual auto update() -> void = 0;
    };
}

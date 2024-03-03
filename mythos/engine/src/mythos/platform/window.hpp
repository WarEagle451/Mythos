#pragma once
#include <myl/definitions.hpp>
#include <myl/math/vec2.hpp>

#include <memory>

/// MYTODO: Window configuration
/// - Implement window states on creation. Add fullscreen
/// - Window is resizeable by user
/// - Window is moveable by user
/// - Window has decoration
/// - Window is given focus on creation
/// - Window can be created on different monitors
/// - Limit window size, small and big
/// - Load window icon
/// - Restore a window to windowed mode
/// - window state change callbacks, windowed, minimized, fullscreen, maximized
/// - Application can tell OS to focus/unfocus on a window
/// - Window attention request like glfw

namespace myth {
    enum class window_state {
        unknown,
        windowed,
        //fullscreen,
        maximized,
        minimized
    };

    struct window_configuration {
        const char* title = "Mythos Application Window";
        //const char* icon = "mythos/resources/branding/logo.png";

        myl::i32vec2 dimensions = { 800, 600 };
        //myl::i32vec2 limit_upper_dimensions = myl::i32vec2(std::numeric_limits<myl::i32>::max());
        //myl::i32vec2 limit_lower_dimensions = myl::i32vec2(0);
        myl::i32vec2 position = myl::i32vec2(-1);
        window_state state = window_state::windowed;
        //myl::i32 monitor = 0;

        //bool moveable = true;
        //bool resizable = true;
        //bool decorated = true;
        //bool gets_focus = true;
        bool center_cursor = true;
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

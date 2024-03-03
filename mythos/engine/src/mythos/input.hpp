#pragma once
#include <mythos/core/keycodes.hpp>
#include <mythos/core/mousecodes.hpp>

#include <myl/math/vec2.hpp>

#include <array>

namespace myth {
    class input {
    public:
        enum class state : bool { up = false, down = true };
    private:
        static std::array<state, key::size> s_key_states;
        static mousecode s_mouse_button_states;

        static myl::f32vec2 s_window_cursor_position;
        static myl::f32vec2 s_cursor_delta;
        static myl::f32vec2 s_scroll_delta;
    public:
        static auto init() -> void;
        static auto update() -> void;
        static auto clear() -> void;

        MYL_NO_DISCARD static MYL_API auto keyboard_state() noexcept -> const std::array<state, key::size>& { return s_key_states; }
        MYL_NO_DISCARD static MYL_API auto key_state(keycode code) noexcept -> state                        { return s_key_states[code]; }
        MYL_NO_DISCARD static MYL_API auto key_up(keycode code) noexcept -> bool                            { return s_key_states[code] == state::up; }
        MYL_NO_DISCARD static MYL_API auto key_down(keycode code) noexcept -> bool                          { return s_key_states[code] == state::down; }

        MYL_NO_DISCARD static MYL_API auto mouse_button_states() noexcept -> mousecode         { return s_mouse_button_states; }
        MYL_NO_DISCARD static MYL_API auto mouse_buttons_up(mousecode code) noexcept -> bool   { return (s_mouse_button_states & code) == code; }
        MYL_NO_DISCARD static MYL_API auto mouse_buttons_down(mousecode code) noexcept -> bool { return (s_mouse_button_states & code) == code; }

        MYL_NO_DISCARD static MYL_API auto cursor_delta() noexcept -> const myl::f32vec2& { return s_cursor_delta; }

        static auto MYL_API set_cursor_position(const myl::i32vec2& position) -> void;
        static auto MYL_API set_cursor_visibility(bool visibility) -> void;
        static auto MYL_API confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void;
        static auto MYL_API release_cursor() -> void;

        static auto MYL_API process_key(keycode code, state state) -> void;
        static auto MYL_API process_typed(myl::u16 character) -> void;
        static auto MYL_API process_mouse_buttons_up(mousecode code) -> void;
        static auto MYL_API process_mouse_buttons_down(mousecode code) -> void;
        static auto MYL_API process_mouse_wheel(const myl::f32vec2& delta) -> void;
        static auto MYL_API process_cursor_delta(const myl::f32vec2& delta) -> void;
        static auto MYL_API process_window_cursor_position(const myl::f32vec2& position) -> void;
    private:
        static auto query_toggleable_key_states(input::state* caps_lock, input::state* num_lock, input::state* scroll_lock) -> void;
    };
}

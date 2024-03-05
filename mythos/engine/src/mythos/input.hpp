#pragma once
#include <mythos/core/keycodes.hpp>
#include <mythos/core/mousecodes.hpp>

#include <myl/math/vec2.hpp>

#include <array>

/// MYTODO: Don't use mousecodes for input, use button_code
/// - mouse_button_states & gamepad_button_states => button_states
/// - mouse_buttons_up & gamepad_buttons_up => button_up
/// - mouse_buttons_down & gamepad_buttons_down => button_down
/// - Maybe even scrap mousecode and replace with a button_code
/// - event::mouse_xxx could be made into button_event

namespace myth {
    using button_code = myl::u32;
    namespace button {
        enum : button_code {
            none = 0,

            // Common buttons for controllers

            left_trigger  = 1 << 0,
            left_bumper   = 1 << 1,
            left_stick    = 1 << 2,
            right_trigger = 1 << 3,
            right_bumper  = 1 << 4,
            right_stick   = 1 << 5,

            up           = 1 << 6,
            down         = 1 << 7,
            left         = 1 << 8,
            right        = 1 << 9,
            symbol_up    = 1 << 10,
            symbol_down  = 1 << 11,
            symbol_left  = 1 << 12,
            symbol_right = 1 << 13,

            start   = 1 << 14,
            options = 1 << 15,
            system  = 1 << 16,

            // Reserved for controller specific support

            auxiliary1 = 1 << 17,
            auxiliary2 = 1 << 18,
            auxiliary3 = 1 << 19,
            auxiliary4 = 1 << 20,
            auxiliary5 = 1 << 21,
            auxiliary6 = 1 << 22,
            auxiliary7 = 1 << 23,
            auxiliary8 = 1 << 24,

            // Platform specific aliases for auxiliary bits

            ps_touchpad = auxiliary1
        };
    }

    struct input_configuration {
        myl::f32vec2 stick_deadzones = { .1f, .1f };
    };

    class input {
    public:
        enum class state : bool { up = false, down = true };
    private:
        static std::array<state, key::size> s_key_states;
        static mousecode s_mouse_button_states;
        static button_code s_gamepad_button_states;

        static myl::f32vec2 s_window_cursor_position;
        static myl::f32vec2 s_cursor_delta;
        static myl::f32vec2 s_scroll_delta;
        static myl::f32vec2 s_stick_deadzones;
        static myl::f32vec2 s_left_stick_delta;
        static myl::f32vec2 s_right_stick_delta;
        static myl::f32vec2 s_trigger_delta;
    public:
        static auto init(const input_configuration& config) -> void;
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

        MYL_NO_DISCARD static MYL_API auto gamepad_button_states() noexcept -> button_code { return s_gamepad_button_states; }
        MYL_NO_DISCARD static MYL_API auto gamepad_buttons_up(button_code code) noexcept -> bool { return (s_gamepad_button_states & code) == code; }
        MYL_NO_DISCARD static MYL_API auto gamepad_buttons_down(button_code code) noexcept -> bool { return (s_gamepad_button_states & code) == code; } 
        MYL_NO_DISCARD static MYL_API auto stick_deadzones() noexcept -> const myl::f32vec2& { return s_stick_deadzones; }
        MYL_NO_DISCARD static MYL_API auto left_stick_delta() noexcept -> const myl::f32vec2& { return s_left_stick_delta; }
        MYL_NO_DISCARD static MYL_API auto right_stick_delta() noexcept -> const myl::f32vec2& { return s_right_stick_delta; }
        MYL_NO_DISCARD static MYL_API auto trigger_deltas() noexcept -> const myl::f32vec2& { return s_trigger_delta; }

        static auto MYL_API set_cursor_position(const myl::i32vec2& position) -> void;
        static auto MYL_API set_cursor_visibility(bool visibility) -> void;
        static auto MYL_API confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void;
        static auto MYL_API release_cursor() -> void;

        static auto process_key(keycode code, state state) -> void;
        static auto process_typed(myl::u16 character) -> void;
        static auto process_mouse_buttons_up(mousecode code) -> void;
        static auto process_mouse_buttons_down(mousecode code) -> void;
        static auto process_mouse_wheel(const myl::f32vec2& delta) -> void;
        static auto process_cursor_delta(const myl::f32vec2& delta) -> void;
        static auto process_window_cursor_position(const myl::f32vec2& position) -> void;

        //static auto process_controller_dualsense3(myl::u8* data, myl::u32 byte_count) -> void;
        //static auto process_controller_dualsense4(myl::u8* data, myl::u32 byte_count) -> void;
        static auto process_controller_dualsense5(myl::u8* data, myl::u32 byte_count) -> void;
        //static auto process_controller_dualsense_edge(myl::u8* data, myl::u32 byte_count) -> void;
    private:
        static auto query_toggleable_key_states(input::state* caps_lock, input::state* num_lock, input::state* scroll_lock) -> void;
    };
}

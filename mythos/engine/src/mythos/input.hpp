#pragma once
#include <mythos/core/keycodes.hpp>
#include <mythos/core/mousecodes.hpp>

#include <myl/math/vec2.hpp>
#include <myl/math/vec3.hpp>
///#include <myl/math/quat.hpp>

#include <array>

/// MYTODO: Up to 4 input devices, Would need to keep track of controllers being plugged in (This would be os dependent)

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
            home  = 1 << 16,

            // Reserved for controller specific support

            auxiliary1 = 1 << 17,
            auxiliary2 = 1 << 18,
            auxiliary3 = 1 << 19,
            auxiliary4 = 1 << 20,
            auxiliary5 = 1 << 21,
            auxiliary6 = 1 << 22,
            auxiliary7 = 1 << 23,
            auxiliary8 = 1 << 24,

            // Platform specific aliases

            ps_triangle = symbol_up,
            ps_circle   = symbol_right,
            ps_cross    = symbol_down,
            ps_square   = symbol_left,
            ps_share    = start,
            ps_create   = start,
            ps_logo     = home,
            ps_touchpad = auxiliary1,
            ps_mic      = auxiliary2,

            xb_y    = symbol_up,
            xb_b    = symbol_right,
            xb_a    = symbol_down,
            xb_x    = symbol_left,
            xb_view = start,
            xb_menu = options,
            xb_logo = home
        };
    }

    struct input_configuration {
        myl::f32vec2 stick_deadzones = { .2f, .2f };
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
        static myl::f32vec2 s_left_stick;
        static myl::f32vec2 s_right_stick;
        static myl::f32vec2 s_trigger_delta;

        ///static myl::f32quat s_gyroscope;
        static myl::f32vec3 s_accelerometer;

        static myl::u32vec2 s_touchpad_touch1_coords;
        static myl::u32vec2 s_touchpad_touch2_coords;
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
        MYL_NO_DISCARD static MYL_API auto scroll_delta() noexcept -> const myl::f32vec2& { return s_scroll_delta; }

        MYL_NO_DISCARD static MYL_API auto gamepad_button_states() noexcept -> button_code { return s_gamepad_button_states; }
        MYL_NO_DISCARD static MYL_API auto gamepad_buttons_up(button_code code) noexcept -> bool { return (s_gamepad_button_states & code) == code; }
        MYL_NO_DISCARD static MYL_API auto gamepad_buttons_down(button_code code) noexcept -> bool { return (s_gamepad_button_states & code) == code; } 
        MYL_NO_DISCARD static MYL_API auto stick_deadzones() noexcept -> const myl::f32vec2& { return s_stick_deadzones; }
        MYL_NO_DISCARD static MYL_API auto left_stick() noexcept -> const myl::f32vec2& { return s_left_stick; }
        MYL_NO_DISCARD static MYL_API auto right_stick() noexcept -> const myl::f32vec2& { return s_right_stick; }
        MYL_NO_DISCARD static MYL_API auto trigger_deltas() noexcept -> const myl::f32vec2& { return s_trigger_delta; }
        ///MYL_NO_DISCARD static MYL_API auto gyroscope() noexcept -> const myl::f32quat& { return s_gyroscope; }
        MYL_NO_DISCARD static MYL_API auto accelerometer() noexcept -> const myl::f32vec3& { return s_accelerometer; }
        MYL_NO_DISCARD static MYL_API auto touch_pad(myl::u32 finger = 1) noexcept -> const myl::u32vec2& { return finger == 1 ? s_touchpad_touch1_coords : s_touchpad_touch2_coords; }

        static auto MYL_API set_cursor_position(const myl::i32vec2& position) -> void;
        static auto MYL_API set_cursor_visibility(bool visibility) -> void;
        static auto MYL_API set_deadstick_zones(const myl::f32vec2& values) noexcept -> void { s_stick_deadzones = values; }

        static auto MYL_API confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void;
        static auto MYL_API release_cursor() -> void;

        static auto process_key(keycode code, state state) -> void;
        static auto process_typed(myl::u16 character) -> void;
        static auto process_mouse_buttons_up(mousecode code) -> void;
        static auto process_mouse_buttons_down(mousecode code) -> void;
        static auto process_mouse_wheel(const myl::f32vec2& delta) -> void;
        static auto process_cursor_delta(const myl::f32vec2& delta) -> void;
        static auto process_window_cursor_position(const myl::f32vec2& position) -> void;

        static auto process_hid(myl::u16 vendor_id, myl::u16 product_id, myl::u8* data, myl::u32 byte_count) -> void;
    private:
        static auto process_controller_buttons(button_code down) -> void;
        static auto process_dpad(myl::u8 byte, button_code& down) -> void;
        static auto process_controller_sticks(const myl::f32vec2& left, const myl::f32vec2& right) -> void;

        static auto process_controller_dualsense(myl::u8* data, myl::u32 byte_count) -> void;
        static auto process_controller_dualshock4(myl::u8* data, myl::u32 byte_count) -> void;

        static auto query_toggleable_key_states(input::state* caps_lock, input::state* num_lock, input::state* scroll_lock) -> void;
    };
}

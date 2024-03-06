#include <mythos/event/gamepad_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/input.hpp>

/// MYTODO: Split this up into seperate source files
#ifdef MYL_OS_WINDOWS
#   include <Windows.h> // WinUser.h
namespace myth {
    auto input::set_cursor_position(const myl::i32vec2& position) -> void {
        SetCursorPos(position.x, position.y);
    }

    auto input::set_cursor_visibility(bool visibility) -> void {
        CURSORINFO ci{ .cbSize = sizeof(CURSORINFO) };
        GetCursorInfo(&ci);
        if ((ci.flags & CURSOR_SHOWING) != visibility)
            ShowCursor(visibility);
    }

    auto input::confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void {
        RECT rect{
            .left = tl.x,
            .top = tl.y,
            .right = br.x,
            .bottom = br.y
        };
        ClipCursor(&rect);
    }

    auto input::release_cursor() -> void {
        ClipCursor(NULL);
    }

    auto input::query_toggleable_key_states(input::state* caps_lock, input::state* num_lock, input::state* scroll_lock) -> void {
        if (caps_lock)
            *caps_lock = (GetKeyState(VK_CAPITAL) & 0x0001) == 0 ? input::state::up : input::state::down;
        if (num_lock)
            *num_lock = (GetKeyState(VK_NUMLOCK) & 0x0001) == 0 ? input::state::up : input::state::down;
        if (scroll_lock)
            *scroll_lock = (GetKeyState(VK_SCROLL) & 0x0001) == 0 ? input::state::up : input::state::down;
    }
}
#else
#   include <mythos/log.hpp>
namespace myth {
    auto input::set_cursor_position(const myl::i32vec2& position) -> void {
        MYTHOS_ERROR("myth::input::set_cursor_position is not available on this platform");
    }

    auto input::set_cursor_visability(bool visable) -> void {
        MYTHOS_ERROR("myth::input::set_cursor_visability is not available on this platform");
    }

    auto input::confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void {
        MYTHOS_ERROR("myth::input::confine_cursor is not available on this platform");
    }

    auto input::unconfine_cursor() -> void {
        MYTHOS_ERROR("myth::input::unconfine_cursor is not available on this platform");
    }

    auto input::query_toggleable_key_states(input::state* caps_lock, input::state* num_lock, input::state* scroll_lock) -> void {
        MYTHOS_ERROR("myth::input::query_toggleable_key_states is not available on this platform");    
    }
}
#endif

#define MYTHOS_IMPL_GAMEPAD_BUTTON(byte, bit, button)\
    if ((bytes[byte] >> bit) != (s_gamepad_button_states & button))\
        ((bytes[byte] >> bit) & 1) ? changed_to_down_buttons |= button : changed_to_up_buttons |= button;

namespace myth {
    std::array<input::state, key::size> input::s_key_states;
    mousecode input::s_mouse_button_states;
    button_code input::s_gamepad_button_states;

    myl::f32vec2 input::s_window_cursor_position;
    myl::f32vec2 input::s_cursor_delta;
    myl::f32vec2 input::s_scroll_delta;
    myl::f32vec2 input::s_stick_deadzones;
    myl::f32vec2 input::s_left_stick_delta;
    myl::f32vec2 input::s_right_stick_delta;
    myl::f32vec2 input::s_trigger_delta;

    auto input::init(const input_configuration& config) -> void {
        clear();
        s_stick_deadzones = config.stick_deadzones;

        /// MYTODO: Should probably query everything that is not a delta
        query_toggleable_key_states(&s_key_states[key::caps_lock], &s_key_states[key::num_lock], &s_key_states[key::scroll_lock]);
    }

    auto input::update() -> void {

    }

    auto input::clear() -> void {
        s_key_states.fill(state::up);
        s_mouse_button_states = mouse_button::none;
        s_gamepad_button_states = button::none;

        s_window_cursor_position = { 0.f, 0.f };
        s_cursor_delta = { 0.f, 0.f };
        s_scroll_delta = { 0.f, 0.f };
        s_stick_deadzones = { 0.f, 0.f };
        s_left_stick_delta = { 0.f, 0.f };
        s_right_stick_delta = { 0.f, 0.f };
        s_trigger_delta = { 0.f, 0.f };
    }

    auto input::process_key(keycode code, state state) -> void {
        if (code == key::unknown)
            return;

        if (s_key_states[code] != state) { // Pressed (Non held) and released events should only be fired on key state change
            s_key_states[code] = state;

            if (state == state::up) {
                event::key_released e(code);
                event::fire(e);
            }
            else {
                event::key_pressed e(code, false);
                event::fire(e);
            }
        }
        else if (state == state::down) { // Key is held down
            event::key_pressed e(code, true);
            event::fire(e);
        }
    }

    auto input::process_typed(myl::u16 character) -> void {
        /// MYTODO: Allow a wider ranger of characters
        if (character < 32 || (character > 126 && character < 160))
            return;
        event::typed e(character);
        event::fire(e);
    }

    auto input::process_mouse_buttons_up(mousecode code) -> void {
        // new = 0110; old = 1010
        // old & new = 0010. Therefore bit 2 has changed state to up
        mousecode changed_buttons = s_mouse_button_states & code;
        if (changed_buttons != 0) { // Only update if there is a change
            s_mouse_button_states &= ~changed_buttons; // Clear bits to up
            event::mouse_released e(changed_buttons);
            event::fire(e);
        }
    }

    auto input::process_mouse_buttons_down(mousecode code) -> void {
        // new = 0110; old = 1010
        // ~(old | ~new) = 0100. Therefore bit 3 has changed state to down
        mousecode changed_buttons = ~(s_mouse_button_states | ~code); // Getting changed down buttons
        if (changed_buttons != 0) { // Only update if there is a change
            s_mouse_button_states |= changed_buttons; // Set bits to down
            event::mouse_pressed e(changed_buttons);
            event::fire(e);
        }
    }

    auto input::process_mouse_wheel(const myl::f32vec2& delta) -> void {
        s_scroll_delta = delta;
        event::mouse_scrolled e(s_scroll_delta);
        event::fire(e);
    }

    auto input::process_cursor_delta(const myl::f32vec2& delta) -> void {
        // Only update if the cursor delta has changed
        if (delta.x != 0.f || delta.y != 0.f)
            s_cursor_delta = delta;
    }

    auto input::process_window_cursor_position(const myl::f32vec2& position) -> void {
        if (s_window_cursor_position != position) {
            s_window_cursor_position = position;
            event::mouse_moved e(s_window_cursor_position);
            event::fire(e);
        }
    }

    auto input::process_hid(myl::u16 vendor_id, myl::u16 product_id, myl::u8* data, myl::u32 byte_count) -> void {
        switch (vendor_id) {
            case 0x54C: // Sony
                /// MYTODO: Process other Sony devices
                switch (product_id) {
                    case 0x05C4: // Dualshock 4 Generation 1
                        break;
                    case 0x09CC: // Dualshock 4 Generation 2
                        break;
                    case 0x0BA0: // Dualshock 3 Wireless Adaptor
                        break;
                    case 0x0CE6: // DualSense 5
                        process_controller_dualsense5(data, byte_count);
                        break;
                    case 0x0CDA: // PlayStation Classic Controller
                        break;
                    case 0x0DF2: // DualSense Edge
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    auto input::process_controller_dualsense5(myl::u8* bytes, myl::u32 byte_count) -> void {
        // From: https://github.com/nondebug/dualsense

        // USB connection is 64 bytes, bluetooth uses more
        const myl::u32 offset = byte_count == 64 ? 3 : 0;

        myl::f32vec2 incoming_left_stick_delta{
            static_cast<myl::f32>(static_cast<myl::u16>(bytes[1]) - 0x80) / 128.f,
            static_cast<myl::f32>(static_cast<myl::u16>(bytes[2]) - 0x80) / 128.f
        };

        if (incoming_left_stick_delta.x > s_stick_deadzones[0] ||
            incoming_left_stick_delta.x < -s_stick_deadzones[0] ||
            incoming_left_stick_delta.y > s_stick_deadzones[0] ||
            incoming_left_stick_delta.y < -s_stick_deadzones[0])
            s_left_stick_delta = incoming_left_stick_delta;

        myl::f32vec2 incoming_right_stick_delta{
            static_cast<myl::f32>(static_cast<myl::u16>(bytes[3]) - 0x80) / 128.f,
            static_cast<myl::f32>(static_cast<myl::u16>(bytes[4]) - 0x80) / 128.f
        };

        if (incoming_right_stick_delta.x > s_stick_deadzones[1] ||
            incoming_right_stick_delta.x < -s_stick_deadzones[1] ||
            incoming_right_stick_delta.y > s_stick_deadzones[1] ||
            incoming_right_stick_delta.y < -s_stick_deadzones[1])
            s_right_stick_delta = incoming_right_stick_delta;

        s_trigger_delta[0] = static_cast<myl::f32>(static_cast<myl::u8>(bytes[8 - offset])) / 255.f;
        s_trigger_delta[1] = static_cast<myl::f32>(static_cast<myl::u8>(bytes[9 - offset])) / 255.f;

        // Used in MYTHOS_IMPL_GAMEPAD_BUTTON
        button_code changed_to_up_buttons = button::none;
        button_code changed_to_down_buttons = button::none;

        MYTHOS_IMPL_GAMEPAD_BUTTON(5 + offset, 4, button::symbol_left);
        MYTHOS_IMPL_GAMEPAD_BUTTON(5 + offset, 5, button::symbol_down);
        MYTHOS_IMPL_GAMEPAD_BUTTON(5 + offset, 6, button::symbol_right);
        MYTHOS_IMPL_GAMEPAD_BUTTON(5 + offset, 7, button::symbol_up);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 0, button::left_bumper);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 1, button::right_bumper);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 2, button::left_trigger);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 3, button::right_trigger);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 4, button::start);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 5, button::options);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 6, button::left_stick);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 7, button::right_stick);
        MYTHOS_IMPL_GAMEPAD_BUTTON(7 + offset, 0, button::system);
        MYTHOS_IMPL_GAMEPAD_BUTTON(7 + offset, 1, button::ps_touchpad);

        /// MYTODO: D-Pad switch not working correctly
        //switch (bytes[5 + offset] & 0xF) { // D-Pad
        //using namespace button;
        //    case 0x0:
        //        changed_to_down_buttons |= up;
        //        changed_to_up_buttons |= down | left | right;
        //        break;
        //    case 0x1:
        //        changed_to_down_buttons |= up | right;
        //        changed_to_up_buttons |= down | left;
        //        break;
        //    case 0x2:
        //        changed_to_down_buttons |= right;
        //        changed_to_up_buttons |= up | down | left;
        //        break;
        //    case 0x3:
        //        changed_to_down_buttons |= down | right;
        //        changed_to_up_buttons |= up | left;
        //        break;
        //    case 0x4:
        //        changed_to_down_buttons |= down;
        //        changed_to_up_buttons |= up | left | right;
        //        break;
        //    case 0x5:
        //        changed_to_down_buttons |= down | left;
        //        changed_to_up_buttons |= up | right;
        //        break;
        //    case 0x6:
        //        changed_to_down_buttons |= left;
        //        changed_to_up_buttons |= up | down | right;
        //        break;
        //    case 0x7:
        //        changed_to_down_buttons |= up | left;
        //        changed_to_up_buttons |= down | right;
        //        break;
        //}

        // Refer too process_mouse_buttons_down
        changed_to_down_buttons = ~(s_gamepad_button_states | ~changed_to_down_buttons);
        if (changed_to_down_buttons != 0) { // Only update if there is a change
            s_gamepad_button_states |= changed_to_down_buttons; // Set bits to down
            event::gamepad_button_pressed e(changed_to_down_buttons);
            event::fire(e);
        }

        // Refer too process_mouse_buttons_up
        changed_to_up_buttons = s_gamepad_button_states & changed_to_up_buttons;
        if (changed_to_up_buttons != 0) { // Only update if there is a change
            s_gamepad_button_states &= ~changed_to_up_buttons; // Clear bits to up
            event::gamepad_button_released e(changed_to_up_buttons);
            event::fire(e);
        }

        /// Below can not be found in bluetooth mode so far

        /// Missing inputs
        /// - Microphone
        /// - Headset Jack
        /// - Battery Status
        /// - Mute button on bluetooth

        // Outputs
        /// - Haptic Feedback
        /// - Adaptive Triggers
        /// - Light Bar
        /// - Speaker
        /// - Headset Jack

        //int i = 00;
        //MYTHOS_TRACE(
        //    "Byte {:3}: {:08b} | Byte {:3}: {:08b} | Byte {:3}: {:08b} | Byte {:3}: {:08b} | Byte {:3}: {:08b} | Byte {:3}: {:08b} | Byte {:3}: {:08b} | Byte {:3}: {:08b} | Byte {:3}: {:08b} | Byte {:3}: {:08b}",
        //    i + 0, bytes[i + 0],
        //    i + 1, bytes[i + 1],
        //    i + 2, bytes[i + 2],
        //    i + 3, bytes[i + 3],
        //    i + 4, bytes[i + 4],
        //    i + 5, bytes[i + 5],
        //    i + 6, bytes[i + 6],
        //    i + 7, bytes[i + 7],
        //    i + 8, bytes[i + 8],
        //    i + 9, bytes[i + 9]
        //);

        //if ((bytes[offset + 7] >> 2) & 1) MYTHOS_WARN("Mute Button"); /// Is this correct for bluetooth? no

        /// Unknown Data
        /// - Byte 7 (8 Bits) - Sometype of counter, increments everytime hid recieves input
        /// - Byte 10 Bit 3 (13 Bits)
        /// - Byte 12 Bit 0 (52 Bytes)

        /// X Byte Location for usb
        /// - 07: ? Increments everytime hid recieves input
        /// - 11: ?
        /// - 12: ? Increments everytime hid recieves input
        /// - 13: Time counter in seconds
        /// - 14: Amount of Roll over from 13
        /// - 15: ? Amount of Roll over from 14
        /// - 16: ? Accel
        /// - 17: ? Accel down
        /// - 18: ? Accel 
        /// - 19: ? Accel to right
        /// - 20: ? Accel Spin
        /// - 21: ? Accel
        /// - 22: ?
        /// - 23: Gyro Roll
        /// - 24: ?
        /// - 25: ? Gyro Yaw
        /// - 26: ?
        /// - 27: Gyro Pitch
        /// - 28: ?
        /// - 29: ?
        /// - 30: ? Some type of counter
        /// - 31: ? Some type of counter
        /// - 32: ?
        /// - 33: Touchpad - 1st input - Bit 0 = is touch pad touched - first input | Bits 1-7 = Touch count tracker
        /// - 34: ? Touchpad - 1st input -
        /// - 35: ? Touchpad - 1st input -
        /// - 36: Touchpad - 1st input - Touch Y Axis: Top = 0, bottom = 67
        /// - 37: Touchpad - 2nd input - Bit 0 = is touch pad touched - first input | Bits 1-7 = Touch count tracker
        /// - 38: ? Touchpad - 2nd input -
        /// - 39: ? Touchpad - 2nd input -
        /// - 40: Touchpad - 2nd input - Touch Y Axis: Top = 0, bottom = 67
        /// - 41: ? Something with Touchpad Regardless of input number
        /// - 42: ?
        /// - 43: ?
        /// - 44: ?
        /// - 45: ?
        /// - 46: ?
        /// - 47: ?
        /// - 48: ?
        /// - 49: ?
        /// - 50: ?
        /// - 51: ? Some type of counter
        /// - 52: Roll over Counter for 51
        /// - 53: ?
        /// - 54: ?
        /// - 55: ?
        /// - 56: ?
        /// - 57: ?
        /// - 58: ?
        /// - 59: ?
        /// - 60: ?
        /// - 61: ?
        /// - 62: ?
        /// - 63: ?
    }
}

#include <mythos/event/gamepad_event.hpp>
#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/input.hpp>

/// MYTEMP:
#include <mythos/log.hpp>
#define MYTHOS_ENABLE_CONTROLLER_WIP

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

/// MYTODO: Implement Controllers
/// - Dualshock 3
///     - https://github.com/felis/USB_Host_Shield_2.0/wiki/PS3-Information
/// - DualSense Edge
/// - Xbox 360
/// - Sixaxis
/// - Xbox Wireless Controller
///     - Models: 1537, 1697, 1698 "Elite", 1708, 1797 "Elite 2", 1914
/// - Google Stadia
/// - Luna Controller

/// MYTODO: Additional Dualsense resources
/// - https://gist.github.com/Nielk1/6d54cc2c00d2201ccb8c2720ad7538db
/// - https://controllers.fandom.com/wiki/Sony_DualSense

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

    myl::u32vec2 input::s_trackpad_touch1_coords;
    myl::u32vec2 input::s_trackpad_touch2_coords;

    auto input::init(const input_configuration& config) -> void {
        clear();
        s_stick_deadzones = config.stick_deadzones;

        /// MYTODO: Should probably query everything that is not a delta
        query_toggleable_key_states(&s_key_states[key::caps_lock], &s_key_states[key::num_lock], &s_key_states[key::scroll_lock]);
    }

    auto input::update() -> void {
        s_cursor_delta = { 0.f, 0.f };
        s_scroll_delta = { 0.f, 0.f };
        s_left_stick_delta = { 0.f, 0.f };
        s_right_stick_delta = { 0.f, 0.f };
        s_trigger_delta = { 0.f, 0.f };
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
        if (changed_buttons != 0) {
            s_mouse_button_states &= ~changed_buttons; // Clear bits
            event::mouse_released e(changed_buttons);
            event::fire(e);
        }
    }

    auto input::process_mouse_buttons_down(mousecode code) -> void {
        // new = 0110; old = 1010
        // ~(old | ~new) = 0100. Therefore bit 3 has changed state to down
        mousecode changed_buttons = ~(s_mouse_button_states | ~code);
        if (changed_buttons != 0) {
            s_mouse_button_states |= changed_buttons; // Set bits
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
            case 0x18D1: // Google
                ///if (product_id == 0x9400) // Stadia Controller
                ///    process_controller_stadia(data, byte_count);
                break;
            case 0x045E: // Microsoft
                switch (product_id) {
                    ///case 0x0202: process_controller_(data, byte_count); break; // Xbox Controller
                    ///case 0x0285: process_controller_(data, byte_count); break; // Xbox Controller S
                    ///case 0x0288: process_controller_(data, byte_count); break; // Xbox Controller S Hub
                    ///case 0x0289: process_controller_(data, byte_count); break; // Xbox Controller S 
                    ///case 0x028E: process_controller_(data, byte_count); break; // Xbox360 Controller
                    ///case 0x028F: process_controller_(data, byte_count); break; // Xbox360 Wireless Contoller via Plug & Charge Cable
                    ///case 0x02D1: process_controller_(data, byte_count); break; // Xbox One Controller
                    ///case 0x02DD: process_controller_(data, byte_count); break; // Xbox One Controller (Firmware 2015)
                    ///case 0x02E0: process_controller_(data, byte_count); break; // Xbox One Wireless Controller
                    ///case 0x02E3: process_controller_(data, byte_count); break; // Xbox One Elite Controller
                    ///case 0x02EA: process_controller_(data, byte_count); break; // Xbox One Controller
                    ///case 0x02FD: process_controller_(data, byte_count); break; // Xbox One S Controller (Bluetooth)
                    ///case 0x0B00: process_controller_(data, byte_count); break; // Xbox Elite Series 2 Controller (model 1797)
                    ///case 0x0B12: process_controller_(data, byte_count); break; // Xbox Controller
                    default: break;
                }
                break;
            case 0x054C: // Sony
                switch (product_id) {
                    case 0x05C4: MYL_FALLTHROUGH;
                    case 0x09CC: process_controller_dualshock4(data, byte_count); break;
                    case 0x0CE6: process_controller_dualsense(data, byte_count); break;
                    ///case 0x0DF2: // Dualsense Edge 
                    default: break;
                }
                break;
            default: break;
        }
    }

    auto input::process_controller_buttons(button_code up, button_code down) -> void {
        // Refer to process_mouse_buttons_down for explanation
        button_code changed_to_down = ~(s_gamepad_button_states | ~down);
        if (changed_to_down != button::none) {
            s_gamepad_button_states |= changed_to_down;
            event::gamepad_button_pressed e(changed_to_down);
            event::fire(e);
        }

        // Refer to process_mouse_buttons_up for explanation
        button_code changed_to_up = s_gamepad_button_states & up;
        if (changed_to_up != button::none) {
            s_gamepad_button_states &= ~changed_to_up;
            event::gamepad_button_released e(changed_to_up);
            event::fire(e);
        }
    }

    auto input::process_controller_sticks(const myl::f32vec2& left, const myl::f32vec2& right) -> void {
        if (left.x > s_stick_deadzones[0] ||
            left.x < -s_stick_deadzones[0] ||
            left.y > s_stick_deadzones[0] ||
            left.y < -s_stick_deadzones[0])
            s_left_stick_delta = left;

        if (right.x > s_stick_deadzones[1] ||
            right.x < -s_stick_deadzones[1] ||
            right.y > s_stick_deadzones[1] ||
            right.y < -s_stick_deadzones[1])
            s_right_stick_delta = right;
    }

    auto input::process_ps_dpad(myl::u8 byte, button_code& up, button_code& down) -> void {
        switch (byte) {
            case 0x0:
                down |= button::up;
                up |= button::down | button::left | button::right;
                break;
            case 0x1:
                down |= button::up | button::right;
                up |= button::down | button::left;
                break;
            case 0x2:
                down |= button::right;
                up |= button::up | button::down | button::left;
                break;
            case 0x3:
                down |= button::down | button::right;
                up |= button::up | button::left;
                break;
            case 0x4:
                down |= button::down;
                up |= button::up | button::right | button::left;
                break;
            case 0x5:
                down |= button::down | button::left;
                up |= button::up | button::right;
                break;
            case 0x6:
                down |= button::left;
                up |= button::up | button::down | button::right;
                break;
            case 0x7:
                down |= button::up | button::left;
                up |= button::down | button::right;
                break;
            case 0x8:
                up |= button::up | button::down | button::left | button::right;
                break;
        }
    }

#define MYTHOS_IMPL_GAMEPAD_BUTTON(byte, bit, button) ((data[byte] >> bit) & 1) ? down |= button : up |= button;

    auto input::process_controller_dualsense(myl::u8* data, myl::u32 byte_count) -> void {
        /// MYTODO: DualSense Controller
        /// - Mute Button on Bluetooth (USB is byte 10, bit 2)

        // USB connection is 64 bytes, bluetooth uses more
        const myl::u32 offset = byte_count == 64 ? 3 : 0;

        myl::f32vec2 left_stick{
            static_cast<myl::f32>(static_cast<myl::u16>(data[1]) - 0x80) / 128.f,
            static_cast<myl::f32>(static_cast<myl::u16>(data[2]) - 0x80) / 128.f
        };

        myl::f32vec2 right_stick{
            static_cast<myl::f32>(static_cast<myl::u16>(data[3]) - 0x80) / 128.f,
            static_cast<myl::f32>(static_cast<myl::u16>(data[4]) - 0x80) / 128.f
        };

        process_controller_sticks(left_stick, right_stick);

        s_trigger_delta[0] = static_cast<myl::f32>(static_cast<myl::u8>(data[8 - offset])) / 255.f;
        s_trigger_delta[1] = static_cast<myl::f32>(static_cast<myl::u8>(data[9 - offset])) / 255.f;

        button_code up = button::none;
        button_code down = button::none;
        MYTHOS_IMPL_GAMEPAD_BUTTON(5 + offset, 4, button::ps_square);
        MYTHOS_IMPL_GAMEPAD_BUTTON(5 + offset, 5, button::ps_cross);
        MYTHOS_IMPL_GAMEPAD_BUTTON(5 + offset, 6, button::ps_circle);
        MYTHOS_IMPL_GAMEPAD_BUTTON(5 + offset, 7, button::ps_triangle);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 0, button::left_bumper);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 1, button::right_bumper);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 2, button::left_trigger);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 3, button::right_trigger);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 4, button::ps_create);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 5, button::options);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 6, button::left_stick);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6 + offset, 7, button::right_stick);
        MYTHOS_IMPL_GAMEPAD_BUTTON(7 + offset, 0, button::ps_logo);
        MYTHOS_IMPL_GAMEPAD_BUTTON(7 + offset, 1, button::ps_touchpad);

#ifdef MYTHOS_ENABLE_CONTROLLER_WIP
        /// MYBUG: Does not work in bluetooth mode
        MYTHOS_IMPL_GAMEPAD_BUTTON(7 + offset, 2, button::ps_mic);
#endif
        process_ps_dpad(data[5 + offset] & 0xF, up, down);

#ifdef MYTHOS_ENABLE_CONTROLLER_WIP
        /// MYBUG: Does not work in bluetooth mode, I think you have to use a outreport to request the controller to send that information
        s_trackpad_touch1_coords = {
            static_cast<myl::u32>(data[31 + offset]) | static_cast<myl::u32>(data[32 + offset] & 0x0F) << 8,
            (static_cast<myl::u32>(data[33 + offset]) << 4) | ((data[32 + offset] & 0xF0) >> 4)
        };

        /// MYBUG: Does not work in bluetooth mode, I think you have to use a outreport to request the controller to send that information
        s_trackpad_touch2_coords = {
            static_cast<myl::u32>(data[35 + offset]) | static_cast<myl::u32>(data[36 + offset] & 0x0F) << 8,
            (static_cast<myl::u32>(data[39 + offset]) << 4) | ((data[36 + offset] & 0xF0) >> 4)
        };
       /// MYTHOS_ERROR("[{}, {}]", s_trackpad_touch1_coords.x, s_trackpad_touch1_coords.y);
#endif
        process_controller_buttons(up, down);

        /// Below can not be found in bluetooth mode so far

        /// Missing inputs
        /// - Microphone
        /// - Headset Jack
        /// - Battery Status

        // Outputs
        /// - Haptic Feedback
        /// - Adaptive Triggers
        /// - Light Bar
        /// - Speaker
        /// - Headset Jack

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
        /// - 24: Gyro Roll
        /// - 25: Gyro Yaw
        /// - 26: Gyro Yaw
        /// - 27: Gyro Pitch
        /// - 28: Gyro Pitch
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

    auto input::process_controller_dualshock4(myl::u8* data, myl::u32 byte_count) -> void {
        /// https://www.psdevwiki.com/ps4/DS4-BT
        /// https://www.psdevwiki.com/ps4/DS4-USB

        /// MYTODO: byte_count is unused

        /// MYTODO: Additional Dualshock inputs
        ///                             USB            Bluetooth
        /// Battery                  : Byte[12] Bit[-] | Byte[15] Bit[-]
        /// Gyro X                   : Byte[13 - 14]   | Byte[16 - 17]
        /// Gyro Y                   : Byte[15 - 16]   | Byte[18 - 19]
        /// Gyro Z                   : Byte[17 - 18]   | Byte[20 - 21]
        /// Accel X (signed)         : Byte[19 - 20]   | Byte[22 - 23]
        /// Accel Y (signed)         : Byte[21 - 22]   | Byte[24 - 25]
        /// Accel Z (signed)         : Byte[23 - 24]   | Byte[26 - 27]
        /// AUX connection           : Byte[30] Bit[-] | Byte[33] Bit[-] ? Is this the Same
        /// Touchpad finger 1 touch  : Byte[35] Bit[-] | Byte[] Bit[] // Refer to https://www.psdevwiki.com/ps4/DS4-BT for Trackpad info
        /// Touchpad Data finger 1   : Byte[36 - 38]   | Byte[] Bit[]
        /// Touchpad finger 2 touch  : Byte[39] Bit[-] | Byte[] Bit[]
        /// Touchpad Data finger 2   : Byte[40 - 42]   | Byte[] Bit[]
        /// Touchpad More Data fin 1 : Byte[44 - 47]   | Byte[] Bit[]
        /// Touchpad More Data fin 2 : Byte[48 - 51]   | Byte[] Bit[]

        myl::f32vec2 left_stick{
            static_cast<myl::f32>(static_cast<myl::u16>(data[1]) - 0x80) / 128.f,
            static_cast<myl::f32>(static_cast<myl::u16>(data[2]) - 0x80) / 128.f
        };

        myl::f32vec2 right_stick{
            static_cast<myl::f32>(static_cast<myl::u16>(data[3]) - 0x80) / 128.f,
            static_cast<myl::f32>(static_cast<myl::u16>(data[4]) - 0x80) / 128.f
        };

        process_controller_sticks(left_stick, right_stick);

        s_trigger_delta[0] = static_cast<myl::f32>(static_cast<myl::u8>(data[8])) / 255.f;
        s_trigger_delta[1] = static_cast<myl::f32>(static_cast<myl::u8>(data[9])) / 255.f;

        button_code up = button::none;
        button_code down = button::none;
        MYTHOS_IMPL_GAMEPAD_BUTTON(5, 4, button::ps_square);
        MYTHOS_IMPL_GAMEPAD_BUTTON(5, 5, button::ps_cross);
        MYTHOS_IMPL_GAMEPAD_BUTTON(5, 6, button::ps_circle);
        MYTHOS_IMPL_GAMEPAD_BUTTON(5, 7, button::ps_triangle);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6, 0, button::left_bumper);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6, 1, button::right_bumper);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6, 2, button::left_trigger);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6, 3, button::right_trigger);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6, 4, button::ps_share);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6, 5, button::options);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6, 6, button::left_stick);
        MYTHOS_IMPL_GAMEPAD_BUTTON(6, 7, button::right_stick);
        MYTHOS_IMPL_GAMEPAD_BUTTON(7, 0, button::ps_logo);
        MYTHOS_IMPL_GAMEPAD_BUTTON(7, 1, button::ps_touchpad);

        process_ps_dpad(data[5] & 0xF, up, down);

#ifdef MYTHOS_ENABLE_CONTROLLER_WIP
        /// MYBUG: Does not work in bluetooth mode, I think you have to use a outreport to request the controller to send that information
        s_trackpad_touch1_coords = {
            static_cast<myl::u32>(data[36]) | static_cast<myl::u32>(data[37] & 0x0F) << 8,
            (static_cast<myl::u32>(data[38]) << 4) | ((data[37] & 0xF0) >> 4)
        };

        /// MYBUG: Does not work in bluetooth mode, I think you have to use a outreport to request the controller to send that information
        s_trackpad_touch2_coords = {
            static_cast<myl::u32>(data[40]) | static_cast<myl::u32>(data[41] & 0x0F) << 8,
            (static_cast<myl::u32>(data[42]) << 4) | ((data[41] & 0xF0) >> 4)
        };
        //MYTHOS_ERROR("[{}, {}]", s_trackpad_touch1_coords.x, s_trackpad_touch1_coords.y);
#endif
        process_controller_buttons(up, down);
    }
}

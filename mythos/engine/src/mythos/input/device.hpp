#pragma once
#include <mythos/input/button.hpp>
#include <mythos/input/keyboard.hpp>

#include <myl/math/vec2.hpp>

#include <array>
#include <string>

namespace myth {
    using native_device_handle_type = void*;

    struct device_usage {
        myl::u16 page;
        myl::u16 id;
    };

    struct device_info {
        std::string  name       = "";
        myl::u16     vendor_id  = 0x0000;
        myl::u16     product_id = 0x0000;
        myl::u16     revision   = 0x0000;
        device_usage usage      = { 0x0000, 0x0000 };
        ///myl::u8 collection = 0x00;
        /// Hardware Version
        /// Firmware Version
        
        myl::u32 input_report_length   = 0;
        myl::u32 output_report_length  = 0;
        myl::u32 feature_report_length = 0;
    };

    struct keyboard_state {
        std::array<key_state, key::size> keys; /// MYTODO: Should probably be replaced with a static bitset

        /// MYTODO: mouse_state
        /// - Pressure
    };

    struct mouse_state {
        mouse_buttons buttons      = mouse_button::none;
        myl::f32vec2  position     = { 0.f, 0.f };
        myl::f32vec2  cursor_delta = { 0.f, 0.f };
        myl::f32vec2  scroll_delta = { 0.f, 0.f };

        /// MYTODO: mouse_state
        /// - Pressure
    };

    struct gamepad_state {
        gamepad_buttons buttons        = gamepad_button::none;
        myl::f32vec2    left_stick     = { 0.f, 0.f };
        myl::f32vec2    right_stick    = { 0.f, 0.f };
        myl::f32vec2    stick_deadzone = { .2f, .2f };
        myl::f32vec2    trigger_deltas = { 0.f, 0.f };

        /// MYTODO: gamepad_state
        /// - haptic
        /// - touchpad (put in device?)
        /// - accelerameter
        /// - gyro
    };

    struct device {
        native_device_handle_type handle = nullptr;
        device_info               info   = device_info{};
        gamepad_state             state  = gamepad_state{}; /// MYTODO: This might not always be a gamepad

        // Defined in a platform specific source file: "platform/[Platform]_device.cpp"

        static auto create(device* device) -> bool;
        static auto destroy(device* device) -> void;
    };
}

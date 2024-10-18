#pragma once
#include <mythos/core/hid_button_code.hpp>

#include <myl/math/vec2.hpp>

#include <memory>

// https://usb.org/sites/default/files/hut1_5.pdf

/// MYTODO: I don't think this is a good way of handling input devices

namespace myth::hid {
    /// MYTODO: Improve HID, so that upcasting is not a trusted thing

    using device_features = myl::usize;
    namespace device_feature {
        enum : device_features {
            none = 0,

            touchpad = 1 << 0
        };
    }

    struct device_base {
        using data_processing_callback = void(*)(device_base* device, myl::u8* data, myl::u32 byte_count);
        using id_type = myl::usize;
    protected:
    public:
        data_processing_callback const processing_callback;
        const id_type                  id;
        const myl::u16                 vendor_id;
        const myl::u16                 product_id;
        const device_features          features = device_feature::none; /// MYTODO: Implement

        MYL_NO_DISCARD device_base(id_type id, myl::u16 vendor_id, myl::u16 product_id, data_processing_callback callback = nullptr);
    };

    // Feature structs

    struct buttons {
        hid_button_code button_states = hid_button::none;
    };

    struct touchpad {

    };

    // General peripheral types

    struct gamepad : public device_base, public buttons {
        myl::f32vec2 left_stick{ 0.f, 0.f };
        myl::f32vec2 right_stick{ 0.f, 0.f };
        myl::f32vec2 trigger_deltas{ 0.f, 0.f };

        myl::f32vec2 stick_deadzones{ .1f, .1f };

        MYL_NO_DISCARD gamepad(id_type id, myl::u16 vendor_id, myl::u16 product_id, data_processing_callback callback = nullptr);
    };

    // Peripheral data processing callbacks

    auto dualshock4_controller_data_processing_callback(device_base* device, myl::u8* data, myl::u32 byte_count) -> void;
    ///auto dualsense_controller_data_processing_callback(device_base* device, myl::u8* data, myl::u32 byte_count) -> void;

    // Peripheral data structures

    struct dualshock4_controller : public gamepad, public touchpad {
        MYL_NO_DISCARD dualshock4_controller(id_type id, myl::u16 vendor_id, myl::u16 product_id);
    };

    auto create(device_base::id_type id, myl::u16 vendor_id, myl::u16 product_id) -> std::unique_ptr<device_base>;
}

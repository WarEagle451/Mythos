#pragma once
#include <mythos/core/hid_button_code.hpp>

#include <myl/math/vec2.hpp>

#include <memory>

// https://usb.org/sites/default/files/hut1_5.pdf

// Useful logging calls for debugging
//MYTHOS_DEBUG("Byte Count: {}", byte_count);
//MYTHOS_DEBUG("Left Stick: [{}, {}]", controller->left_stick.x, controller->left_stick.y);
//MYTHOS_DEBUG("Right Stick: [{}, {}]", controller->right_stick.x, controller->right_stick.y);
//MYTHOS_DEBUG("Buttons: {:0>{}b}", controller->button_states, sizeof(hid_button_code) * 8);
//MYTHOS_DEBUG("Trigger Deltas: [{}, {}]", controller->trigger_deltas.left, controller->trigger_deltas.right);
//MYTHOS_DEBUG("Touchpad Touch 1: x= {}, y={}", controller->touch1.x, controller->touch1.y);
//MYTHOS_DEBUG("Touchpad Touch 2: x= {}, y={}", controller->touch2.x, controller->touch2.y);

namespace myth::hid {
    using device_features = myl::usize;
    namespace device_feature {
        enum : device_features { /// MYTODO: implement, is this the correct design to prevent trusted upcasting?
            none = 0,

            buttons       = 1 << 0,
            touchpad      = 1 << 1,
            accelerometer = 1 << 2,
            gyroscope     = 1 << 3
        };
    }

    struct device {
        using data_processing_callback = void(*)(device* handle, myl::u8* data, myl::u32 byte_count);
        using id_type = myl::usize;
    public:
        data_processing_callback const processing_callback;
        const id_type                  id;
        const myl::u16                 vendor_id;
        const myl::u16                 product_id;
        const device_features          features = device_feature::none; /// MYTODO: Implement

        MYL_NO_DISCARD device(id_type id, myl::u16 vendor_id, myl::u16 product_id, data_processing_callback callback = nullptr);
    };

    // Feature structs

    struct buttons {
        hid_button_code button_states = hid_button::none;
    };

    struct touchpad {
        bool         touch1_active = false;
        myl::u32vec2 touch1 = myl::u32vec2{ 0, 0 };
        bool         touch2_active = false;
        myl::u32vec2 touch2 = myl::u32vec2{ 0, 0 };
    };

    // General peripheral types

    struct gamepad : public device, public buttons {
        myl::f32vec2 left_stick{ 0.f, 0.f };
        myl::f32vec2 right_stick{ 0.f, 0.f };
        myl::f32vec2 trigger_deltas{ 0.f, 0.f };

        myl::f32vec2 stick_deadzones{ .1f, .1f };

        MYL_NO_DISCARD gamepad(id_type id, myl::u16 vendor_id, myl::u16 product_id, data_processing_callback callback = nullptr);
    };

    auto create(device::id_type id, myl::u16 vendor_id, myl::u16 product_id) -> std::unique_ptr<device>;
}

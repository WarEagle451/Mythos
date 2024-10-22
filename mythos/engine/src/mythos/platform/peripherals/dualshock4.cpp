#include <mythos/input.hpp>
#include <mythos/platform/peripherals/dualshock4.hpp>

namespace myth::hid {
    auto dualshock4_controller_data_processing_callback(device* handle, myl::u8* data, myl::u32 byte_count) -> void {
        // USB Data Format from https://www.psdevwiki.com/ps4/DS4-USB & https://www.psdevwiki.com/ps4/DS4-BT

        /// MYTODO: Dualshock4 features
        /// - Battery
        /// - Haptic feedback
        /// - Light bar
        /// - Touchpad in bluetooth mode

        using namespace hid_button;
        dualshock4_controller* controller = static_cast<dualshock4_controller*>(handle);
        const bool is_usb_device = byte_count == 64;

        const myl::f32vec2 l_stick{
            static_cast<myl::f32>(myl::u16(data[1]) - 0x80) / 128.f,
            static_cast<myl::f32>(myl::u16(data[2]) - 0x80) / 128.f
        };

        controller->left_stick = myl::length(l_stick) > controller->stick_deadzones.left ?
            l_stick :
            myl::f32vec2(0.f);

        const myl::f32vec2 r_stick{
            static_cast<myl::f32>(myl::u16(data[3]) - 0x80) / 128.f,
            static_cast<myl::f32>(myl::u16(data[4]) - 0x80) / 128.f
        };

        controller->right_stick = myl::length(r_stick) > controller->stick_deadzones.right ?
            r_stick :
            myl::f32vec2(0.f);

        hid_button_code buttons_down = none;

        if ((data[5] >> 4) & 1) buttons_down |= ps_square;
        if ((data[5] >> 5) & 1) buttons_down |= ps_cross;
        if ((data[5] >> 6) & 1) buttons_down |= ps_circle;
        if ((data[5] >> 7) & 1) buttons_down |= ps_triangle;

        switch (data[5] & 0x0F) {
        case 0x0: buttons_down |= up;           break;
        case 0x1: buttons_down |= up | right;   break;
        case 0x2: buttons_down |= right;        break;
        case 0x3: buttons_down |= down | right; break;
        case 0x4: buttons_down |= down;         break;
        case 0x5: buttons_down |= down | left;  break;
        case 0x6: buttons_down |= left;         break;
        case 0x7: buttons_down |= up | left;    break;
        }

        if ((data[6] >> 0) & 1) buttons_down |= left_bumper;
        if ((data[6] >> 1) & 1) buttons_down |= right_bumper;
        if ((data[6] >> 2) & 1) buttons_down |= left_trigger;
        if ((data[6] >> 3) & 1) buttons_down |= right_trigger;
        if ((data[6] >> 4) & 1) buttons_down |= ps_share;
        if ((data[6] >> 5) & 1) buttons_down |= options;
        if ((data[6] >> 6) & 1) buttons_down |= left_stick;
        if ((data[6] >> 7) & 1) buttons_down |= right_stick;

        if ((data[7] >> 0) & 1) buttons_down |= ps_logo;
        if ((data[7] >> 1) & 1) buttons_down |= ps_touchpad;

        input::process_hid_buttons(handle, static_cast<buttons*>(controller), buttons_down);

        controller->trigger_deltas.left = static_cast<myl::f32>(static_cast<myl::u8>(data[8])) / 255.f;
        controller->trigger_deltas.right = static_cast<myl::f32>(static_cast<myl::u8>(data[9])) / 255.f;

        /// MYTODO: Handle gyroscope
        /// - Gyro X: angular velocity measures (follows right-hand-rule) | Byte: 13-14 |
        /// - Gyro Y                                                      | Byte: 15-16 |
        /// - Gyro Z                                                      | Byte: 17-18 |    

        //myl::u16* gyro_data = static_cast<myl::u16*>(static_cast<void*>(&data[13]));
        //controller->gyro = {
        //
        //};

        /// MYTODO: Handle accelerometer
        /// - Accel X (signed): acceleration (positive: right)            | Byte: 19-20 |
        /// - Accel Y (signed): acceleration (positive: up)               | Byte: 21-22 |
        /// - Accel Z (signed): acceleration (positive: towards player)   | Byte: 23-24 |       

        //myl::i16* accelerator_data = static_cast<myl::i16*>(static_cast<void*>(&data[19]));
        //controller->acceleration = {
        //    static_cast<myl::i32>(*accelerator_data),
        //    static_cast<myl::i32>(*(accelerator_data + 1)),
        //    static_cast<myl::i32>(*(accelerator_data + 2))
        //};

        if (is_usb_device) {
            controller->touch1_active = (data[35] & 0B1000'0000) == 0;
            if (controller->touch1_active)
                controller->touch1 = {
                    static_cast<myl::u16>(data[36]) | (static_cast<myl::u16>(data[37] & 0x0F) << 8),
                    (static_cast<myl::u16>(data[38]) << 4) | static_cast<myl::u16>(((data[37] & 0xF0) >> 4))
            };

            controller->touch2_active = (data[39] & 0B1000'0000) == 0;
            if (controller->touch2_active)
                controller->touch2 = {
                    static_cast<myl::u16>(data[40]) | (static_cast<myl::u16>(data[41] & 0x0F) << 8),
                    (static_cast<myl::u16>(data[42]) << 4) | static_cast<myl::u16>(((data[41] & 0xF0) >> 4))
            };
        }
        else { // Bluetooth
            /// MYTODO: Touchpad in bluetooth mode, maybe this has to be enabled as a separate feature
        }
    }

    MYL_NO_DISCARD dualshock4_controller::dualshock4_controller(id_type id, myl::u16 vendor_id, myl::u16 product_id)
        : gamepad(id, vendor_id, product_id, (data_processing_callback)dualshock4_controller_data_processing_callback)
        , touchpad{} {

    }
}

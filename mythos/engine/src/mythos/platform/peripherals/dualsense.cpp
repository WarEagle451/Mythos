#include <mythos/input.hpp>
#include <mythos/platform/peripherals/dualsense.hpp>

/// Additional controller resources
/// - https://github.com/Ohjurot/DualSense-Windows
/// - https://www.reddit.com/r/Dualsense/comments/pl3fsb/code_to_generate_all_dualsense_trigger_effects/
/// - https://gist.github.com/Nielk1/6d54cc2c00d2201ccb8c2720ad7538db
/// - https://patchwork.kernel.org/project/linux-input/patch/20201219062336.72568-6-roderick@gaikai.com/
/// - https://controllers.fandom.com/wiki/Sony_DualSense

namespace myth::hid {
    auto dualsense_controller_data_processing_callback(device* handle, myl::u8* data, myl::u32 byte_count) -> void {
        // USB Data Format from https://github.com/nondebug/dualsense

        /// MYTODO: Dualsense features
        /// - Battery
        /// - Haptic feedback
        /// - Adaptive triggers
        /// - Light bar
        /// - Touchpad in bluetooth mode

        using namespace hid_button;
        dualsense_controller* controller = static_cast<dualsense_controller*>(handle);
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

        if (is_usb_device) {
            controller->trigger_deltas.left = static_cast<myl::f32>(static_cast<myl::u8>(data[5])) / 255.f;
            controller->trigger_deltas.right = static_cast<myl::f32>(static_cast<myl::u8>(data[6])) / 255.f;

            switch (data[8] & 0x0F) {
                case 0x0: buttons_down |= up;           break;
                case 0x1: buttons_down |= up | right;   break;
                case 0x2: buttons_down |= right;        break;
                case 0x3: buttons_down |= down | right; break;
                case 0x4: buttons_down |= down;         break;
                case 0x5: buttons_down |= down | left;  break;
                case 0x6: buttons_down |= left;         break;
                case 0x7: buttons_down |= up | left;    break;
            }

            if ((data[8] >> 4) & 1) buttons_down |= ps_square;
            if ((data[8] >> 5) & 1) buttons_down |= ps_cross;
            if ((data[8] >> 6) & 1) buttons_down |= ps_circle;
            if ((data[8] >> 7) & 1) buttons_down |= ps_triangle;

            if ((data[9] >> 0) & 1) buttons_down |= left_bumper;
            if ((data[9] >> 1) & 1) buttons_down |= right_bumper;
            if ((data[9] >> 2) & 1) buttons_down |= left_trigger;
            if ((data[9] >> 3) & 1) buttons_down |= right_trigger;
            if ((data[9] >> 4) & 1) buttons_down |= ps_create;
            if ((data[9] >> 5) & 1) buttons_down |= options;
            if ((data[9] >> 6) & 1) buttons_down |= left_stick;
            if ((data[9] >> 7) & 1) buttons_down |= right_stick;

            if ((data[10] >> 0) & 1) buttons_down |= ps_logo;
            if ((data[10] >> 1) & 1) buttons_down |= ps_touchpad;
            if ((data[10] >> 2) & 1) buttons_down |= ps_mute;

            /// MYTODO: Handle gyroscope

            /// MYTODO: Handle accelerometer

            controller->touch1_active = (data[33] & 0B1000'0000) == 0;
            if (controller->touch1_active)
                controller->touch1 = {
                    static_cast<myl::u16>(data[34]) | (static_cast<myl::u16>(data[35] & 0x0F) << 8),
                    (static_cast<myl::u16>(data[36]) << 4) | static_cast<myl::u16>(((data[35] & 0xF0) >> 4))
            };

            controller->touch2_active = (data[37] & 0B1000'0000) == 0;
            if (controller->touch2_active)
                controller->touch2 = {
                    static_cast<myl::u16>(data[38]) | (static_cast<myl::u16>(data[39] & 0x0F) << 8),
                    (static_cast<myl::u16>(data[40]) << 4) | static_cast<myl::u16>(((data[39] & 0xF0) >> 4))
            };
        }
        else { // Bluetooth
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

            if ((data[5] >> 4) & 1) buttons_down |= ps_square;
            if ((data[5] >> 5) & 1) buttons_down |= ps_cross;
            if ((data[5] >> 6) & 1) buttons_down |= ps_circle;
            if ((data[5] >> 7) & 1) buttons_down |= ps_triangle;

            if ((data[6] >> 0) & 1) buttons_down |= left_bumper;
            if ((data[6] >> 1) & 1) buttons_down |= right_bumper;
            if ((data[6] >> 2) & 1) buttons_down |= left_trigger;
            if ((data[6] >> 3) & 1) buttons_down |= right_trigger;
            if ((data[6] >> 4) & 1) buttons_down |= ps_create;
            if ((data[6] >> 5) & 1) buttons_down |= options;
            if ((data[6] >> 6) & 1) buttons_down |= left_stick;
            if ((data[6] >> 7) & 1) buttons_down |= right_stick;

            if ((data[7] >> 0) & 1) buttons_down |= ps_logo;
            if ((data[7] >> 1) & 1) buttons_down |= ps_touchpad;
            // ps_mute is not available in bluetooth mode

            controller->trigger_deltas.left = static_cast<myl::f32>(static_cast<myl::u8>(data[8])) / 255.f;
            controller->trigger_deltas.right = static_cast<myl::f32>(static_cast<myl::u8>(data[9])) / 255.f;

            /// MYTODO: Handle gyroscope

            /// MYTODO: Handle accelerometer

            /// MYTODO: Handle touchpad
        }

        input::process_hid_buttons(handle, static_cast<buttons*>(controller), buttons_down);
    }

    MYL_NO_DISCARD dualsense_controller::dualsense_controller(id_type id, myl::u16 vendor_id, myl::u16 product_id)
        : gamepad(id, vendor_id, product_id, (data_processing_callback)dualsense_controller_data_processing_callback)
        , touchpad{} {

    }
}

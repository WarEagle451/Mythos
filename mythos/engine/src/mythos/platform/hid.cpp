#include <mythos/input.hpp>
#include <mythos/log.hpp>
#include <mythos/platform/hid.hpp>

/// MYTODO: If device can not find a processing callback it should fail the add device in input.cpp

/// MYTODO: HID implementaion
/// - Dualshock 3: https://github.com/felis/USB_Host_Shield_2.0/wiki/PS3-Information
/// - DualSense
/// - DualSense Edge
/// - Xbox 360
/// - Sixaxis
/// - Xbox Wireless Controller
///     - Models: 1537, 1697, 1698 "Elite", 1708, 1797 "Elite 2", 1914
/// - Google Stadia
///     - line 215 onwards ? https://github.com/71/stadiacontroller/blob/master/src/stadia.rs
///     - https://github.com/helloparthshah/StadiaWireless
/// - Luna Controller

/// MYTODO: Additional Dualsense resources
/// - https://gist.github.com/Nielk1/6d54cc2c00d2201ccb8c2720ad7538db
/// - https://controllers.fandom.com/wiki/Sony_DualSense

namespace myth::hid {
    static auto deduce_hid_data_processing_callback(myl::u16 vendor_id, myl::u16 product_id) -> device_base::data_processing_callback {
        switch (vendor_id) {
            case 0x54C: // Sony
                switch (product_id) {
                    case 0x05C4: MYL_FALLTHROUGH; // Dualshock 4
                    case 0x09CC: return dualshock4_controller_data_processing_callback;
                    ///case 0x0CE6: return dualsense_controller_processing_callback;
                    default: break;
                }
                break;
            default: break;
        }

        MYTHOS_ERROR("Device HID processing callback can't be determined. Vendor ID: {}, Product ID: {}", vendor_id, product_id);
        /// MYTODO: If a specific process can't be determined at this point then rely on the products intended usage, eg gamepad, mouse, etc, and try a default layout

        MYTHOS_FATAL("No device HID processing callback could be found, application will likey crash when device attempts to send a HID report. Vendor ID: {}, Product ID: {}", vendor_id, product_id);
        return nullptr;
    }

    MYL_NO_DISCARD device_base::device_base(id_type id, myl::u16 vendor_id, myl::u16 product_id, data_processing_callback callback)
        : processing_callback{ callback == nullptr ? deduce_hid_data_processing_callback(vendor_id, product_id) : callback }
        , id{ id }
        , vendor_id{ vendor_id }
        , product_id{ product_id }
        , features{} { /// MYTODO: Implement features

    }

    MYL_NO_DISCARD gamepad::gamepad(id_type id, myl::u16 vendor_id, myl::u16 product_id, data_processing_callback callback)
        : device_base(id, vendor_id, product_id, callback)
        , buttons{} {

    }

    // Peripheral data processing callbacks

    auto dualshock4_controller_data_processing_callback(device_base* device, myl::u8* data, MYL_MAYBE_UNUSED myl::u32 byte_count) -> void {
        /// MYTODO: byte_count is unused, is bluetooth the same as usb?

        /// MYTODO: Dualshock4 controller data:
        /// Feature                                                       | USB         |          | Bluetooth
        /// - Battery level                                               | Byte: 12    |    

        // USB Data Format from https://www.psdevwiki.com/ps4/DS4-USB & https://www.psdevwiki.com/ps4/DS4-BT

        using namespace hid_button;
        dualshock4_controller* controller = static_cast<dualshock4_controller*>(device);       

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
        
        switch (data[5] & 0xF) {
            case 0x0: buttons_down |= up; break;
            case 0x1: buttons_down |= up | right; break;
            case 0x2: buttons_down |= right; break;
            case 0x3: buttons_down |= down | right; break;
            case 0x4: buttons_down |= down; break;
            case 0x5: buttons_down |= down | left; break;
            case 0x6: buttons_down |= left; break;
            case 0x7: buttons_down |= up | left; break;
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
        
        input::process_hid_buttons(device, static_cast<buttons*>(controller), buttons_down);

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

        /// MYTODO: Handle accelerator
        /// - Accel X (signed): acceleration (positive: right)            | Byte: 19-20 |
        /// - Accel Y (signed): acceleration (positive: up)               | Byte: 21-22 |
        /// - Accel Z (signed): acceleration (positive: towards player)   | Byte: 23-24 |
        
        //myl::i16* accelerator_data = static_cast<myl::i16*>(static_cast<void*>(&data[19]));
        //controller->acceleration = {
        //    accelerator_data,
        //    accelerator_data + 1,
        //    accelerator_data + 2,
        //};

        /// MYTODO: Handle touchpad
        /// - T-PAD event active:                                         | Byte: 33    | Bit: 0-3 |
        /// - T-PAD: tracking numbers for touch count for finger 1        | Byte: 35    | Bit: 0-6 |
        /// - Finger down on 1st touch                                    | Byte: 35    | Bit: 7   |
        /// - T-PAD: each finger 1 location/positional data:              | Byte: 36-38 |

        //controller->touchpad_finger1 = {
        //    ,
        //
        //};

        /// - T-PAD: tracking numbers for touch count for finger 2        | Byte: 39    | Bit: 0-6 |
        /// - Finger down on 2nd touch                                    | Byte: 39    | Bit: 7   |
        /// - T-PAD: each finger 2 location/positional data:              | Byte: 40-42 |
        /// - T-PAD: the previous touches 1st finger track and location   | Byte: 44-47 |
        /// - T-PAD: the previous touches 2nd finger track and location   | Byte: 48-51 |

        //controller->touchpad_finger2 = {
        //    ,
        //
        //};
    }

    // Peripheral data structures

    MYL_NO_DISCARD dualshock4_controller::dualshock4_controller(id_type id, myl::u16 vendor_id, myl::u16 product_id)
        : gamepad(id, vendor_id, product_id, (data_processing_callback)dualshock4_controller_data_processing_callback)
        , touchpad{} {

    }

    auto create(device_base::id_type id, myl::u16 vendor_id, myl::u16 product_id) -> std::unique_ptr<device_base> {
        switch (vendor_id) {
            case 0x54C: // Sony
                switch (product_id) {
                    case 0x05C4: MYL_FALLTHROUGH;                                                      // Dualshock 4
                    case 0x09CC: return std::make_unique<gamepad>(id, vendor_id, product_id, nullptr); // Dualshock 4
                    case 0x0CE6: return std::make_unique<gamepad>(id, vendor_id, product_id, nullptr); // DualSense
                    ///case 0x0DF2:  // Dualsense Edge 
                    default:
                        MYTHOS_ERROR("Unknown Sony device detected, product ID: {}", product_id);
                        break;
                } break;
            default:
                MYTHOS_ERROR("Unknown device detected, vendor ID: {}, product ID: {}", vendor_id, product_id);
                break;
        }

        return nullptr;
    }
}

/// Beyond is now bad

#if 0
namespace myth {
#define MYTHOS_IMPL_GAMEPAD_BUTTON(byte, bit, button)\
    if ((data[byte] >> bit) & 1)\
        buttons_down |= button;

    auto input::process_controller_dualsense(myl::u8* data, myl::u32 byte_count) -> void {
        // https://github.com/nondebug/dualsense

        /// MYTODO: Additional controller resources
        /// - https://github.com/Ohjurot/DualSense-Windows
        /// - https://www.reddit.com/r/Dualsense/comments/pl3fsb/code_to_generate_all_dualsense_trigger_effects/
        /// - https://gist.github.com/Nielk1/6d54cc2c00d2201ccb8c2720ad7538db
        /// - https://patchwork.kernel.org/project/linux-input/patch/20201219062336.72568-6-roderick@gaikai.com/

        // USB connection is 64 bytes, bluetooth uses more
        const myl::u32 offset = byte_count == 64 ? 3 : 0;

        const myl::f32vec2 left_stick{
            static_cast<myl::f32>(static_cast<myl::u16>(data[1]) - 0x80) / 128.f,
            static_cast<myl::f32>(static_cast<myl::u16>(data[2]) - 0x80) / 128.f
        };

        const myl::f32vec2 right_stick{
            static_cast<myl::f32>(static_cast<myl::u16>(data[3]) - 0x80) / 128.f,
            static_cast<myl::f32>(static_cast<myl::u16>(data[4]) - 0x80) / 128.f
        };

        process_controller_sticks(left_stick, right_stick);

        s_trigger_delta[0] = static_cast<myl::f32>(static_cast<myl::u8>(data[8 - offset])) / 255.f;
        s_trigger_delta[1] = static_cast<myl::f32>(static_cast<myl::u8>(data[9 - offset])) / 255.f;

        button_code buttons_down = button::none;
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
        process_dpad(data[5 + offset] & 0xF, buttons_down);

        if (offset == 3) // The DuelSense's mute button can only be detected in usb mode
            MYTHOS_IMPL_GAMEPAD_BUTTON(10, 2, button::ps_mic);

        process_controller_buttons(buttons_down);

#ifdef MYTHOS_ENABLE_CONTROLLER_WIP
        /// MYBUG: Does not work in bluetooth mode, I think you have to use a outreport to request the controller to send that information
        s_touchpad_touch1_coords = {
            static_cast<myl::u32>(data[31 + offset]) | static_cast<myl::u32>(data[32 + offset] & 0x0F) << 8,
            (static_cast<myl::u32>(data[33 + offset]) << 4) | ((data[32 + offset] & 0xF0) >> 4)
        };

        /// MYBUG: Does not work in bluetooth mode, I think you have to use a outreport to request the controller to send that information
        s_touchpad_touch2_coords = {
            static_cast<myl::u32>(data[35 + offset]) | static_cast<myl::u32>(data[36 + offset] & 0x0F) << 8,
            (static_cast<myl::u32>(data[39 + offset]) << 4) | ((data[36 + offset] & 0xF0) >> 4)
        };
        /// MYTHOS_ERROR("[{}, {}]", s_trackpad_touch1_coords.x, s_trackpad_touch1_coords.y);
#endif

#ifdef MYTHOS_ENABLE_CONTROLLER_WIP
        //s_gyroscope = myl::f32quat(
        //    w,
        //    static_cast<myl::f32>((static_cast<myl::u16>(data[20 + offset]) << 8) | static_cast<myl::u16>(data[21 + offset])),
        //    static_cast<myl::f32>((static_cast<myl::u16>(data[22 + offset]) << 8) | static_cast<myl::u16>(data[23 + offset])),
        //    static_cast<myl::f32>((static_cast<myl::u16>(data[24 + offset]) << 8) | static_cast<myl::u16>(data[25 + offset]))
        //);

        //s_accelerometer = myl::f32vec3{
        //    static_cast<myl::f32>((static_cast<myl::i16>(data[13 + offset]) << 8) | static_cast<myl::i16>(data[14 + offset])),
        //    static_cast<myl::f32>((static_cast<myl::i16>(data[15 + offset]) << 8) | static_cast<myl::i16>(data[16 + offset])),
        //    static_cast<myl::f32>((static_cast<myl::i16>(data[17 + offset]) << 8) | static_cast<myl::i16>(data[18 + offset]))
        //};
        //MYTHOS_ERROR("Accelerometer: [{}, {}, {}]", s_accelerometer.x, s_accelerometer.y, s_accelerometer.z);
#endif

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
        /// - 37: Touchpad - 2nd input - Bit 0 = is touch pad touched - first input | Bits 1-7 = Touch count tracker
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
#endif

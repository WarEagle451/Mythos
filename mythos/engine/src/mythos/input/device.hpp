#pragma once 
#include <myl/math/vec2.hpp>

/// Design Idea!!!
/// - Launch a thread that continously updates a buffer that contains the controllers data, every report |= the data in case of buttons.
///   This then gets processed once a frame and send to input controls. However maybe this is bad for events. and Maybe the new thread is not a good idea

/// event polling seems to be the best way to handle this. then after update abunch of values
/// https://www.reddit.com/r/gameenginedevs/comments/11mve1y/need_advices_on_my_input_system_design/
/// https://www.gamedeveloper.com/design/the-5-golden-rules-of-input

/// Resources:
/// - https://usb.org/sites/default/files/hut1_5.pdf

/// THIS MAYBE BE THE BACK END IM LOOKING FOR 
///  https://learn.microsoft.com/en-us/windows-hardware/drivers/hid/

/// - https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/gameinput_members
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputdevicecapabilities
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputdevicefamily
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputdevicestatus
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputenumerationkind
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputfeedbackaxes
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputfeedbackeffectstate
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputfocuspolicy
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputforcefeedbackeffectkind
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputkeyboardkind
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputlabel
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputlocation
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputmotionaccuracy
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputrawdeviceitemcollectionkind
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputrawdevicephysicalunitkind
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputrawdevicereportitemflags
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputrawdevicereportkind
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputrumblemotors
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputswitchkind
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputswitchposition
// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/enums/gameinputtouchshape

namespace myth::device {
    using input_kinds = myl::u16;
    namespace input_kind {
        enum : input_kinds {
            unknown           = 0,
            raw_device_report = 1 << 0,
            controller_axis   = 1 << 1,
            controller_button = 1 << 2,
            controller_switch = 1 << 3,
            controller        = 1 << 4,
            keyboard          = 1 << 5,
            mouse             = 1 << 6,
            touch             = 1 << 7,
            motion            = 1 << 8,
            arcade_stick      = 1 << 9,
            flight_stick      = 1 << 10,
            gamepad           = 1 << 11,
            racing_wheel      = 1 << 12,
            ui_navigation     = 1 << 13
        };
    }

    using button_type = myl::u32;
    namespace button {
        enum : myl::u32 {
            none     = 0,
            button0  = 1 << 0,
            button1  = 1 << 1,
            button2  = 1 << 2,
            button3  = 1 << 3,
            button4  = 1 << 4,
            button5  = 1 << 5,
            button6  = 1 << 6,
            button7  = 1 << 7,
            button8  = 1 << 8,
            button9  = 1 << 9,
            button10 = 1 << 10,
            button11 = 1 << 11,
            button12 = 1 << 12,
            button13 = 1 << 13,
            button14 = 1 << 14,
            button15 = 1 << 15,
            button16 = 1 << 16,
            button17 = 1 << 17,
            button18 = 1 << 18,
            button19 = 1 << 19,
            button20 = 1 << 20,
            button21 = 1 << 21,
            button22 = 1 << 22,
            button23 = 1 << 23,
            button24 = 1 << 24,
            button25 = 1 << 25,
            button26 = 1 << 26,
            button27 = 1 << 27,
            button28 = 1 << 28,
            button29 = 1 << 29,
            button30 = 1 << 30,
            button31 = 1 << 31
        };
    }

    using arcade_stick_buttons = button_type;
    namespace arcade_stick_button {
        enum : arcade_stick_buttons {
            none     = button::none,
            menu     = button::button0,
            view     = button::button1,
            up       = button::button2,
            down     = button::button3,
            left     = button::button4,
            right    = button::button5,
            action1  = button::button6,
            action2  = button::button7,
            action3  = button::button8,
            action4  = button::button9,
            action5  = button::button10,
            action6  = button::button11,
            special1 = button::button12,
            special2 = button::button13
        };
    }

    using flight_stick_buttons = button_type;
    namespace flight_stick_button {
        enum : flight_stick_buttons {
            none           = button::none,
            menu           = button::button0,
            view           = button::button1,
            fire_primary   = button::button2,
            fire_secondary = button::button3
        };
    }

    using gamepad_buttons = button_type;
    namespace gamepad_button {
        enum : gamepad_buttons {
            none         = button::none,
            menu         = button::button0,
            view         = button::button1,
            a            = button::button2,
            b            = button::button3,
            x            = button::button4,
            y            = button::button5,
            dpad_up      = button::button6,
            dpad_down    = button::button7,
            dpad_left    = button::button8,
            dpad_right   = button::button9,
            left_bumper  = button::button10,
            right_bumper = button::button11,
            left_stick   = button::button12,
            right_stick  = button::button13
        };
    }

    using mouse_buttons = button_type;
    namespace mouse_button {
        enum : mouse_buttons {
            none       = button::none,
            left       = button::button0,
            right      = button::button1,
            middle     = button::button2,
            button4    = button::button3,
            button5    = button::button4,
            tilt_left  = button::button5,
            tilt_right = button::button6
        };
    }

    using racing_wheel_buttons = button_type;
    namespace racing_wheel_button {
        enum : racing_wheel_buttons {
            none          = button::none,
            menu          = button::button0,
            view          = button::button1,
            previous_gear = button::button2,
            next_gear     = button::button3,
            dpad_up       = button::button4,
            dpad_down     = button::button5,
            dpad_left     = button::button6,
            dpad_right    = button::button7
        };
    }

    using system_buttons = button_type;
    namespace system_button {
        enum : system_buttons {
            none  = button::none,
            guide = button::button0,
            share = button::button1
        };
    }

    using ui_navigation_buttons = button_type;
    namespace ui_navigation_button {
        enum : ui_navigation_buttons {
            none         = button::none,
            menu         = button::button0,
            view         = button::button1,
            accept       = button::button2,
            cancel       = button::button3,
            up           = button::button4,
            down         = button::button5,
            left         = button::button6,
            right        = button::button7,
            context1     = button::button8,
            context2     = button::button9,
            context3     = button::button10,
            context4     = button::button11,
            page_up      = button::button12,
            page_down    = button::button13,
            page_left    = button::button14,
            page_right   = button::button15,
            scroll_up    = button::button16,
            scroll_down  = button::button17,
            scroll_left  = button::button18,
            scroll_right = button::button19,
        };
    }

    enum class battery_status {
        unknown     = -1,
        not_present = 0,
        discharging = 1,
        idle        = 2,
        charging    = 3
    };

    enum class device_report_kind {
        input_report,
        output_report,
        feature_report
    };

    struct arcade_button_state {
        arcade_stick_buttons buttons;
    };

    struct flight_stick_state {
        flight_stick_buttons buttons;
        /// hat switch https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/structs/gameinputflightstickstate        
        myl::f32             roll;
        myl::f32             pitch;
        myl::f32             yaw;
        myl::f32             throttle;
    };

    struct gamepad_state {
        gamepad_buttons buttons;
        myl::f32vec2  triggers;
        myl::f32vec2  left_stick;
        myl::f32vec2  right_stick;
    };

    struct mouse_state {
        mouse_buttons buttons;
        myl::i64vec2  position;
        myl::i64vec2  wheel;
    };

    struct racing_wheel_state {
        racing_wheel_buttons buttons;
        myl::i32             pattern_shifter_gear;
        myl::f32             wheel;
        myl::f32             throttle;
        myl::f32             brake;
        myl::f32             clutch;
        myl::f32             handbrake;
    };

    struct ui_navigation_state {
        ui_navigation_buttons buttons;
    };

    /// <summary>
    /// 
    /// </summary>

    using usage_page_type = myl::u16;
    using usage_id_type   = myl::u16;

    struct usage {
        usage_page_type page;
        usage_id_type   id;
    };

    struct device_report_info {
        device_report_kind kind;
        myl::u32 id;
        myl::u32 size; /// NEEDED?
        const std::vector<device_report_item_info> items;
    }; /// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/structs/gameinputrawdevicereportinfo

    /// https://learn.microsoft.com/en-us/gaming/gdk/_content/gc/reference/input/gameinput/structs/gameinputdeviceinfo
    struct device_info {
        myl::u16 vendor_id;
        myl::u16 product_id;
        usage usage;

        myl::u32 controller_axis_count;
        myl::u32 controller_button_count;
        myl::u32 controller_switch_count;
        myl::u32 touch_point_count;
        myl::u32 touch_sensor_count;
        myl::u32 force_feedback_motor_count;
        myl::u32 haptic_feedback_motor_count;
    };

    ///

    auto get_input_report() -> device_report_info;
    auto get_output_report() -> device_report_info;
    auto get_feature_report() -> device_report_info;

    ///


}

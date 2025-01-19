#pragma once
#include <myl/definitions.hpp>

namespace mye {
    using button_code = myl::u32;
    using buttons     = button_code;
    namespace button {
        enum : button_code {
            none = 0,

            n0  = 1u << 0,
            n1  = 1u << 1,
            n2  = 1u << 2,
            n3  = 1u << 3,
            n4  = 1u << 4,
            n5  = 1u << 5,
            n6  = 1u << 6,
            n7  = 1u << 7,
            n8  = 1u << 8,
            n9  = 1u << 9,
            n10 = 1u << 10,
            n11 = 1u << 11,
            n12 = 1u << 12,
            n13 = 1u << 13,
            n14 = 1u << 14,
            n15 = 1u << 15,
            n16 = 1u << 16,
            n17 = 1u << 17,
            n18 = 1u << 18,
            n19 = 1u << 19,
            n20 = 1u << 20,
            n21 = 1u << 21,
            n22 = 1u << 22,
            n23 = 1u << 23,
            n24 = 1u << 24,
            n25 = 1u << 25,
            n26 = 1u << 26,
            n27 = 1u << 27,
            n28 = 1u << 28,
            n29 = 1u << 29,
            n30 = 1u << 30,
            n31 = 1u << 31
        };
    }

    using mousecode     = button_code;
    using mouse_buttons = buttons;
    namespace mouse_button {
        enum : mousecode {
            none = button::none,

            left    = button::n0,
            right   = button::n1,
            middle  = button::n2,
            button4 = button::n3,
            button5 = button::n4
        };
    }

    using gamepad_button_code = button_code;
    using gamepad_buttons     = buttons;
    namespace gamepad_button {
        enum : gamepad_button_code {
            none = button::none,
            
            // Common names
            
            action_left   = button::n0,
            action_down   = button::n1,
            action_right  = button::n2,
            action_up     = button::n3,
            left_bumper   = button::n4,
            right_bumper  = button::n5,
            left_trigger  = button::n6,
            right_trigger = button::n7,
            start         = button::n8,
            options       = button::n9,
            left_stick    = button::n10,
            right_stick   = button::n11,
            home          = button::n12,
            
            up    = button::n28,
            down  = button::n29,
            left  = button::n30,
            right = button::n31,
            
            // Playstation aliases
            
            ps_triangle = action_up,
            ps_cross    = action_down,
            ps_square   = action_left,
            ps_circle   = action_right,
            ps_share    = start,
            ps_create   = start,
            ps_logo     = home,
            ps_touchpad = button::n13,
            ps_mute     = button::n14,
            
            // Xbox aliases
            
            xb_y    = action_up,
            xb_a    = action_down,
            xb_x    = action_left,
            xb_b    = action_right,
            xb_view = start,
            xb_menu = options,
            xb_logo = home
        };
    }
}

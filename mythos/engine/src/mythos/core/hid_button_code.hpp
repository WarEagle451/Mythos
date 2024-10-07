#pragma once
#include <myl/definitions.hpp>

namespace myth {
    using hid_button_code = myl::u32;

    namespace hid_button {
        enum : hid_button_code {           
            none = 0,
            size = 32,

            // Buttons

            button0  = 1u << 0,
            button1  = 1u << 1,
            button2  = 1u << 2,
            button3  = 1u << 3,
            button4  = 1u << 4,
            button5  = 1u << 5,
            button6  = 1u << 6,
            button7  = 1u << 7,
            button8  = 1u << 8,
            button9  = 1u << 9,
            button10 = 1u << 10,
            button11 = 1u << 11,
            button12 = 1u << 12,
            button13 = 1u << 13,
            button14 = 1u << 14,
            button15 = 1u << 15,
            button16 = 1u << 16,
            button17 = 1u << 17,
            button18 = 1u << 18,
            button19 = 1u << 19,
            button20 = 1u << 20,
            button21 = 1u << 21,
            button22 = 1u << 22,
            button23 = 1u << 23,
            button24 = 1u << 24,
            button25 = 1u << 25,
            button26 = 1u << 26,
            button27 = 1u << 27,
            button28 = 1u << 28,
            button29 = 1u << 29,
            button30 = 1u << 30,
            button31 = 1u << 31,

            // Common names

            left_trigger  = button0,
            left_bumper   = button1,
            left_stick    = button2,
            right_trigger = button3,
            right_bumper  = button4,
            right_stick   = button5,

            up           = button6,
            down         = button7,
            left         = button8,
            right        = button9,
            symbol_up    = button10,
            symbol_down  = button11,
            symbol_left  = button12,
            symbol_right = button13,

            start   = button14,
            options = button15,
            home    = button16,

            // Platform specific aliases

            ps_triangle = symbol_up,
            ps_cross    = symbol_down,
            ps_square   = symbol_left,
            ps_circle   = symbol_right,
            ps_share    = start,
            ps_create   = start,
            ps_logo     = home,
            ps_touchpad = button31,

            xb_y    = symbol_up,
            xb_a    = symbol_down,
            xb_x    = symbol_left,
            xb_b    = symbol_right,
            xb_view = start,
            xb_menu = options,
            xb_logo = home
        };
    }
}

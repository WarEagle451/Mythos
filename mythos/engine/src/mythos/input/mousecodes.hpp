#pragma once
#include <myl/definitions.hpp>

namespace myth {
    using mousecode = myl::u16;
    namespace mouse_button {
        enum : mousecode {
            none = 0,

            button0 = 1 << 0,
            button1 = 1 << 1,
            button2 = 1 << 2,
            button3 = 1 << 3,
            button4 = 1 << 4,
            button5 = 1 << 5,
            button6 = 1 << 6,
            button7 = 1 << 7,
            button8 = 1 << 8,
            button9 = 1 << 9,
            button10 = 1 << 10,
            button11 = 1 << 11,
            button12 = 1 << 12,
            button13 = 1 << 13,
            button14 = 1 << 14,
            button15 = 1 << 15,

            left   = button0,
            right  = button1,
            middle = button2,
        };
    }
}

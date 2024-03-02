#pragma once
#include <myl/definitions.hpp>

namespace myth {
    using mousecode = myl::u16;
    namespace mouse_button {
        enum : mousecode {
            none = 0,

            button1 = 1 << 0,
            button2 = 1 << 1,
            button3 = 1 << 2,
            button4 = 1 << 3,
            button5 = 1 << 4,
            button6 = 1 << 5,
            button7 = 1 << 6,
            button8 = 1 << 7,

            left   = button1,
            right  = button2,
            middle = button3,
        };
    }
}

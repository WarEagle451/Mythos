#pragma once
#include <mythos/core/keycodes.hpp>

namespace myth {
    MYL_NO_DISCARD constexpr auto ps2_set1_make_scancode_to_keycode(myl::u16 scancode) noexcept -> keycode {
        // From Microsoft
        // See Remarks - USB HID to PS/2 Scan Code Translation Table: https://learn.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-rawkeyboard#remarks
        switch (scancode) {
            using namespace key;
            case 0x01: return escape; // Escape
            case 0x02: return n1; // 1 !
            case 0x03: return n2; // 2 @
            case 0x04: return n3; // 3 #
            case 0x05: return n4; // 4 $
            case 0x06: return n5; // 5 %
            case 0x07: return n6; // 6 ^
            case 0x08: return n7; // 7 &
            case 0x09: return n8; // 8 *
            case 0x0A: return n9; // 9 (
            case 0x0B: return n0; // 0 )
            case 0x0C: return dash; // - _
            case 0x0D: return equals; // = +
            case 0x0E: return backspace; // Backspace
            case 0x0F: return tab; // Tab
            case 0x10: return q; // Q q
            case 0x11: return w; // W w
            case 0x12: return e; // E e
            case 0x13: return r; // R r
            case 0x14: return t; // T t
            case 0x15: return y; // Y y
            case 0x16: return u; // U u
            case 0x17: return i; // I i
            case 0x18: return o; // O o
            case 0x19: return p; // P p
            case 0x1A: return left_bracket; // [ {
            case 0x1B: return right_bracket; // ] }
            case 0x1C: return enter; // Enter
            case 0x1D: return left_control; // Left control
            case 0x1E: return a; // A a
            case 0x1F: return s; // S s
            case 0x20: return d; // D d
            case 0x21: return f; // F f
            case 0x22: return g; // G g
            case 0x23: return h; // H h
            case 0x24: return j; // J j
            case 0x25: return k; // K k
            case 0x26: return l; // L l
            case 0x27: return semicolon; // ; :
            case 0x28: return apostrophe; // ' "
            case 0x29: return grave_accent; // ` ~
            case 0x2A: return left_shift; // Left shift
            //case 0x2B: return unknown; // Europe 1
            case 0x2B: return backslash; // \ |
            case 0x2C: return z; // Z z
            case 0x2D: return x; // X x
            case 0x2E: return c; // C c
            case 0x2F: return v; // V v
            case 0x30: return b; // B b
            case 0x31: return n; // N n
            case 0x32: return m; // M m
            case 0x33: return comma; // , <
            case 0x34: return period; // . >
            case 0x35: return slash; // / ?
            case 0x36: return right_shift; // Right shift
            case 0x37: return kp_multiply; // Keypad *
            case 0x38: return left_alt; // Left alt
            case 0x39: return space; // Space
            case 0x3A: return caps_lock; // Caps lock
            case 0x3B: return f1; // F1
            case 0x3C: return f2; // F2
            case 0x3D: return f3; // F3
            case 0x3E: return f4; // F4
            case 0x3F: return f5; // F5
            case 0x40: return f6; // F6
            case 0x41: return f7; // F7
            case 0x42: return f8; // F8
            case 0x43: return f9; // F9
            case 0x44: return f10; // F10
            case 0x45: return num_lock; // Num Lock
            case 0x46: return scroll_lock; // Scroll lock
            case 0x47: return kp7; // Keypad 7 home
            case 0x48: return kp8; // Keypad 8 up
            case 0x49: return kp9; // Keypad 9 page up
            case 0x4A: return kp_subtract; // Keypad -
            case 0x4B: return kp4; // Keypad 4 left
            case 0x4C: return kp5; // Keypad 5
            case 0x4D: return kp6; // Keypad 6 right
            case 0x4E: return kp_add; // Keypad +
            case 0x4F: return kp1; // Keypad 1 end
            case 0x50: return kp2; // Keypad 2 down
            case 0x51: return kp3; // Keypad 3 page down
            case 0x52: return kp0; // Keypad 0 insert
            case 0x53: return kp_decimal; // Keypad . delete

            case 0x56: return unknown; // Europe 2
            case 0x57: return f11; // F11
            case 0x58: return f12; // F12
            case 0x59: return kp_equals; // Keypad =

            case 0x5C: return unknown; // Keyboard Int'l 6 (PC9800 Keypad , )

            case 0x64: return f13; // F13
            case 0x65: return f14; // F14
            case 0x66: return f15; // F15
            case 0x67: return f16; // F16
            case 0x68: return f17; // F17
            case 0x69: return f18; // F18
            case 0x6A: return f19; // F19
            case 0x6B: return f20; // F20
            case 0x6C: return f21; // F21
            case 0x6D: return f22; // F22
            case 0x6E: return f23; // F23

            case 0x70: return unknown; // Keyboard Intl'2 (Katakana/Hiragana)

            case 0x73: return unknown; // Keyboard Int'l 1 (Ro)

            case 0x76: return f24; // F24
            //case 0x76: return ; // Keyboard Lang 5 (Zenkaku/Hankaku)
            case 0x77: return unknown; // Keyboard Lang 4 Hiragana)
            case 0x78: return unknown; // Keyboard Lang 3 (Katakana)
            case 0x79: return unknown; // Keyboard Int'l 4 (Henkan)

            case 0x7B: return unknown; // Keyboard Int'l 5 (Muhenkan)

            case 0x7D: return unknown; // Keyboard Int'l 2 (Yen)
            case 0x7E: return unknown; // Keypad , (Brazilian Keypad .)

            case 0xF1: return unknown; // Keyboard Lang 2 (Hanja)
            case 0xF2: return unknown; // Keyboard Lang 1 (Hanguel/English)

            case 0xE010: return unknown; // Scan previous track

            case 0xE019: return unknown; // Scan next track

            case 0xE01C: return kp_enter; // Keypad enter
            case 0xE01D: return right_control; // Right control

            case 0xE020: return unknown; // Mute
            case 0xE021: return unknown; // Calculator
            case 0xE022: return unknown; // Play / Pause
            
            case 0xE024: return unknown; // Stop

            case 0xE02E: return unknown; // Volume down

            case 0xE030: return unknown; // Volume up

            case 0xE032: return unknown; // Browser home

            case 0xE035: return kp_divide; // Keypad /

            case 0xE037: return printscreen; // Printscreen
            case 0xE038: return right_alt; // Right alt

            case 0xE046: return pause; // Break (Ctrl-Pause)
            case 0xE047: return home; // Home
            case 0xE048: return up; // Up arrow
            case 0xE049: return page_up; // Page up

            case 0xE04B: return left; // Left arrow

            case 0xE04D: return right; // Right arrow

            case 0xE04F: return end; // End
            case 0xE050: return down; // Down arrow
            case 0xE051: return page_down; // Page down
            case 0xE052: return insert; // Insert
            case 0xE053: return delete_key; // Delete

            case 0xE05B: return left_super; // Left GUI
            case 0xE05C: return right_super; // Right GUI
            case 0xE05D: return unknown; // App
            //case 0xE05E: return unknown; // Keyboard power
            case 0xE05E: return unknown; // System power
            case 0xE05F: return unknown; // System sleep

            case 0xE063: return unknown; // System wake

            case 0xE065: return unknown; // Browser search
            case 0xE066: return unknown; // Browser favorites
            case 0xE067: return unknown; // Browser refresh
            case 0xE068: return unknown; // Browser stop
            case 0xE069: return unknown; // Browser forward
            case 0xE06A: return unknown; // Browser back
            case 0xE06B: return unknown; // My computer
            case 0xE06C: return unknown; // Mail
            case 0xE06D: return unknown; // Media select

            case 0xE0C6: // Break (Ctrl-Pause)

            ///MYBUG: Below makes no sense for a u16 number
            case 0xE19DC5: return pause;

            case 0xE1D145: return pause;
            default: return unknown;
        }
    }
}

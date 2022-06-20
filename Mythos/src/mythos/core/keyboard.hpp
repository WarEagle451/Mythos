#pragma once
#include "key_codes.hpp"

namespace myl {
	MYL_NO_DISCARD constexpr key_code translate_ps2_set_1_make(u16 scancode) noexcept {
		switch (scancode) { // From: https://download.microsoft.com/download/1/6/1/161ba512-40e2-4cc9-843a-923143f3456c/translate.pdf
			using namespace key;
			case 0x00: return unknown; // No Event
			case 0x01: return escape;
			case 0x02: return n1;
			case 0x03: return n2;
			case 0x04: return n3;
			case 0x05: return n4;
			case 0x06: return n5;
			case 0x07: return n6;
			case 0x08: return n7;
			case 0x09: return n8;
			case 0x0A: return n9;
			case 0x0B: return n0;
			case 0x0C: return dash;
			case 0x0D: return equal;
			case 0x0E: return backspace;
			case 0x0F: return tab;
			case 0x10: return q;
			case 0x11: return w;
			case 0x12: return e;
			case 0x13: return r;
			case 0x14: return t;
			case 0x15: return y;
			case 0x16: return u;
			case 0x17: return i;
			case 0x18: return o;
			case 0x19: return p;
			case 0x1A: return left_bracket;
			case 0x1B: return right_bracket;
			case 0x1C: return enter;
			case 0x1D: return left_control;
			case 0x1E: return a;
			case 0x1F: return s;
			case 0x20: return d;
			case 0x21: return f;
			case 0x22: return g;
			case 0x23: return h;
			case 0x24: return j;
			case 0x25: return k;
			case 0x26: return l;
			case 0x27: return semicolon;
			case 0x28: return apostrophe;
			case 0x29: return grave_accent;
			case 0x2A: return left_shift;
			case 0x2B: return backslash;
			case 0x2C: return z;
			case 0x2D: return x;
			case 0x2E: return c;
			case 0x2F: return v;
			case 0x30: return b;
			case 0x31: return n;
			case 0x32: return m;
			case 0x33: return comma;
			case 0x34: return period;
			case 0x35: return slash;
			case 0x36: return right_shift;
			case 0x37: return multiply;
			case 0x38: return left_alt;
			case 0x39: return space;
			case 0x3A: return caps_lock;
			case 0x3B: return f1;
			case 0x3C: return f2;
			case 0x3D: return f3;
			case 0x3E: return f4;
			case 0x3F: return f5;
			case 0x40: return f6;
			case 0x41: return f7;
			case 0x42: return f8;
			case 0x43: return f9;
			case 0x44: return f10;
			case 0x45: return num_lock;
			case 0x46: return scroll_lock;
			case 0x47: return kp7; /// Also Home
			case 0x48: return kp8; /// Also Up
			case 0x49: return kp9; /// Also Page Up
			case 0x4A: return subtract;
			case 0x4B: return kp4; /// Also Left
			case 0x4C: return kp5;
			case 0x4D: return kp6; /// Also Right
			case 0x4E: return add;
			case 0x4F: return kp1; /// Also End
			case 0x50: return kp2; /// Also Down
			case 0x51: return kp3; /// Also Page Down
			case 0x52: return kp0; /// Also Insert
			case 0x53: return kp_decimal; /// Also Delete

			case 0x56: return unknown; // Europe 2
			case 0x57: return f11;
			case 0x58: return f12;
			case 0x59: return kp_equal;

			case 0x5C: return unknown; // International 6

			case 0x64: return f13;
			case 0x65: return f14;
			case 0x66: return f15;
			case 0x67: return f16;
			case 0x68: return f17;
			case 0x69: return f18;
			case 0x6A: return f19;
			case 0x6B: return f20;
			case 0x6C: return f21;
			case 0x6D: return f22;
			case 0x6E: return f23;

			case 0x70: return unknown; // International 2

			case 0x73: return unknown; // International 1

			case 0x76: return f24;
			case 0x77: return unknown; // Lang 4
			case 0x78: return unknown; // Lang 3
			case 0x79: return unknown; // International 4

			case 0x7D: return unknown; // International 2
			case 0x7E: return unknown; // Keypad Comma

			case 0xF1: return unknown; // Lang 2
			case 0xF2: return unknown; // Lang 1

			case 0xFB: return unknown; // International 5
			case 0xFC: return unknown; // POST Fail

			case 0xFF: return unknown; // Overrun Error

			case 0xE010: return unknown; // Previous

			case 0xE019: return unknown; // Next

			case 0xE01C: return kp_enter;
			case 0xE01D: return right_control;

			case 0xE020: return unknown; // Mute
			case 0xE021: return unknown; // Calculator
			case 0xE022: return unknown; // Play/Pause

			case 0xE024: return unknown; // Stop

			case 0xE02E: return unknown; // Volume Down

			case 0xE030: return unknown; // Volume Up

			case 0xE032: return unknown; // Browser Home

			case 0xE035: return divide;

			case 0xE037: return print_screen;
			case 0xE038: return right_alt;

			case 0xE047: return home;
			case 0xE048: return up;
			case 0xE049: return page_up;

			case 0xE04B: return left;

			case 0xE04D: return right;

			case 0xE04F: return end;
			case 0xE050: return down;
			case 0xE051: return page_down;
			case 0xE052: return insert;
			case 0xE053: return delete_key;

			case 0xE05B: return left_super;
			case 0xE05C: return right_super;
			case 0xE05D: return unknown; // App
			case 0xE05E: return unknown; // System Power
			case 0xE05F: return unknown; // System Sleep

			case 0xE063: return unknown; // System Wake

			case 0xE065: return unknown; // Browser Search
			case 0xE066: return unknown; // Browser Favorities
			case 0xE067: return unknown; // Browser Refresh
			case 0xE068: return unknown; // Browser Stop
			case 0xE069: return unknown; // Browser Forward
			case 0xE06A: return unknown; // Browser Back
			case 0xE06B: return unknown; // My Computer
			case 0xE06C: return unknown; // Mail
			case 0xE06D: return unknown; // Media Select

			case 0xE19DC5: return pause;

			case 0xE1D145: return pause;

			default: return unknown;
		}
	}
}
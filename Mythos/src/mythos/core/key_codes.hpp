#pragma once
#include <mythos/defines.hpp>

namespace myl {
	using key_code = u16;
	namespace key {
		enum : key_code {
			tab = 9,
			enter = 13,
			escape = 27,
			space = 32,
			apostrophe = 39,
			comma = 44,
			dash = 45,
			period = 46,
			slash = 47,

			n0 = 48, // 0
			n1 = 49, // 1
			n2 = 50, // 2
			n3 = 51, // 3
			n4 = 52, // 4
			n5 = 53, // 5
			n6 = 54, // 6
			n7 = 55, // 7
			n8 = 56, // 8
			n9 = 57, // 9

			semicolon = 59,
			equal = 61,

			a = 65,
			b = 66,
			c = 67,
			d = 68,
			e = 69,
			f = 70,
			g = 71,
			h = 72,
			i = 73,
			j = 74,
			k = 75,
			l = 76,
			m = 77,
			n = 78,
			o = 79,
			p = 80,
			q = 81,
			r = 82,
			s = 83,
			t = 84,
			u = 85,
			v = 86,
			w = 87,
			x = 88,
			y = 89,
			z = 90,

			left_bracket = 91,
			backslash = 92,
			right_bracket = 93,
			grave_accent = 96,

			backspace = 127,

			// keys below 128 should line up with ansii standards

			world1,
			world2,

			insert,
			delete_key,
			right,
			left,
			down,
			up,
			page_up,
			page_down,
			home,
			end,

			caps_lock,
			scroll_lock,
			num_lock,
			print_screen,
			pause,

			f1,
			f2,
			f3,
			f4,
			f5,
			f6,
			f7,
			f8,
			f9,
			f10,
			f11,
			f12,
			f13,
			f14,
			f15,
			f16,
			f17,
			f18,
			f19,
			f20,
			f21,
			f22,
			f23,
			f24,

			kp0, // keypad 0
			kp1, // keypad 1
			kp2, // keypad 2
			kp3, // keypad 3
			kp4, // keypad 4
			kp5, // keypad 5
			kp6, // keypad 6
			kp7, // keypad 7
			kp8, // keypad 8
			kp9, // keypad 9
			decimal,
			divide,
			multiply,
			subtract,
			add,
			kp_enter,
			kp_equal,

			left_shift,
			left_control,
			left_alt,
			left_super,

			right_shift,
			right_control,
			right_alt,
			right_super,

			menu,

			size,
			unknown
		};
	}
}
#pragma once
#include <mythos/defines.hpp>

namespace myl {
	using key_code = u16;
	namespace key {
		enum : key_code {
			n0, // 0
			n1, // 1
			n2, // 2
			n3, // 3
			n4, // 4
			n5, // 5
			n6, // 6
			n7, // 7
			n8, // 8
			n9, // 9

			a,
			b,
			c,
			d,
			e,
			f,
			g,
			h,
			i,
			j,
			k,
			l,
			m,
			n,
			o,
			p,
			q,
			r,
			s,
			t,
			u,
			v,
			w,
			x,
			y,
			z,

			kp0, // Keypad 0
			kp1, // Keypad 1
			kp2, // Keypad 2
			kp3, // Keypad 3
			kp4, // Keypad 4
			kp5, // Keypad 5
			kp6, // Keypad 6
			kp7, // Keypad 7
			kp8, // Keypad 8
			kp9, // Keypad 9
			decimal,
			divide,
			multiply,
			subtract,
			add,
			kp_enter,
			kp_equal,

			space,
			apostrophe,
			comma,
			dash,
			period,
			slash,
			semicolon,
			equal,
			left_bracket,
			right_bracket,
			backslash,
			grave_accent,

			backspace,
			tab,
			enter,

			right,
			left,
			down,
			up,

			escape,
			insert,
			delete_key,
			page_up,
			page_down,
			home,
			end,
			menu,

			world1,
			world2,

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

			left_shift,
			left_control,
			left_alt,
			left_super,

			right_shift,
			right_control,
			right_alt,
			right_super,

			size,
			unknown
		};
	}
}
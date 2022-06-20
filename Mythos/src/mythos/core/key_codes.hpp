#pragma once
#include <mythos/defines.hpp>

namespace myl {
	using key_code = u16;
	namespace key {
		//@brief Values with assigned numbers are compatible with ANCII standard
		enum : key_code {
			left_shift,
			left_control,
			left_alt,
			left_super,
			right_shift,
			right_control,
			right_alt,
			right_super,
			backspace		= 8,	// \b
			tab				= 9,	// \t
			caps_lock,
			scroll_lock,
			num_lock,
			enter			= 13,	// \r
			kp0,					// Keypad 0
			kp1,					// Keypad 1
			kp2,					// Keypad 2
			kp3,					// Keypad 3
			kp4,					// Keypad 4
			kp5,					// Keypad 5
			kp6,					// Keypad 6
			kp7,					// Keypad 7
			kp8,					// Keypad 8
			kp9,					// Keypad 9
			kp_enter,				// Keypad enter
			kp_equal,				// Keypad equal
			kp_decimal,
			escape			= 27,	// \e
			divide,
			multiply,
			subtract,
			add,
			space			= 32,
			insert,
			delete_key,
			page_up,
			page_down,
			home,
			end,
			apostrophe		= 39,
			right,
			left,
			down,
			up,
			comma			= 44,
			dash			= 45,
			period			= 46,
			slash			= 47,
			n0				= 48,	// 0
			n1				= 49,	// 1
			n2				= 50,	// 2
			n3				= 51,	// 3
			n4				= 52,	// 4
			n5				= 53,	// 5
			n6				= 54,	// 6
			n7				= 55,	// 7
			n8				= 56,	// 8
			n9				= 57,	// 9
			print_screen,
			semicolon		= 59,
			pause,
			equal			= 61,
			f1,
			f2,
			f3,
			a				= 65,
			b				= 66,
			c				= 67,
			d				= 68,
			e				= 69,
			f				= 70,
			g				= 71,
			h				= 72,
			i				= 73,
			j				= 74,
			k				= 75,
			l				= 76,
			m				= 77,
			n				= 78,
			o				= 79,
			p				= 80,
			q				= 81,
			r				= 82,
			s				= 83,
			t				= 84,
			u				= 85,
			v				= 86,
			w				= 87,
			x				= 88,
			y				= 89,
			z				= 90,
			left_bracket	= 91,
			backslash		= 92,
			right_bracket	= 93,
			f4,
			f5,
			grave_accent	= 96,
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

			size,
			unknown
		};
	}

	namespace key_mod {
		enum : key_code {
			none		= 0,
			shift		= 1 << 0,
			control		= 1 << 1,
			alt			= 1 << 2,
			super		= 1 << 3,
			caps_lock	= 1 << 4,
			num_lock	= 1 << 5
		};
	}
}
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

			world1 = 161,
			world2 = 162,

			insert = 260,
			delete_key = 261,
			right = 262,
			left = 263,
			down = 264,
			up = 265,
			page_up = 266,
			page_down = 267,
			home = 268,
			end = 269,

			caps_lock = 280,
			scroll_lock = 281,
			num_lock = 282,
			print_screen = 283,
			pause = 284,

			f1 = 290,
			f2 = 291,
			f3 = 292,
			f4 = 293,
			f5 = 294,
			f6 = 295,
			f7 = 296,
			f8 = 297,
			f9 = 298,
			f10 = 299,
			f11 = 300,
			f12 = 301,
			f13 = 302,
			f14 = 303,
			f15 = 304,
			f16 = 305,
			f17 = 306,
			f18 = 307,
			f19 = 308,
			f20 = 309,
			f21 = 310,
			f22 = 311,
			f23 = 312,
			f24 = 313,

			kp0 = 320, // keypad 0
			kp1 = 321, // keypad 1
			kp2 = 322, // keypad 2
			kp3 = 323, // keypad 3
			kp4 = 324, // keypad 4
			kp5 = 325, // keypad 5
			kp6 = 326, // keypad 6
			kp7 = 327, // keypad 7
			kp8 = 328, // keypad 8
			kp9 = 329, // keypad 9
			decimal = 330,
			divide = 331,
			multiply = 332,
			subtract = 333,
			add = 334,
			kp_enter = 335,
			kp_equal = 336,

			left_shift = 340,
			left_control = 341,
			left_alt = 342,
			left_super = 343,

			right_shift = 344,
			right_control = 345,
			right_alt = 346,
			right_super = 347,

			menu = 348,

			unknown,
			size,

			/// shift,
			/// control,
			/// alt,
			/// super,

			/// convert,
			/// non_convert,
			/// accept,
			/// mode_change,
			/// prior,
			/// next,
			/// select
			/// print
			/// execute
			/// snapshot
			/// help
			/// sleep
			/// apps
			/// separator
			/// l menu
			/// r menu
		};
	}
}
#pragma once
#include <mythos/core/mouse_codes.hpp>
#include <mythos/core/key_codes.hpp>

#include <string>

#define TB_CODE_TO_STRING_CASE(code) case code: return #code

namespace tb {
	MYL_NO_DISCARD constexpr std::string mouse_buttons_to_string(myl::mouse_code code) {
		using namespace myl::mouse_button;
		if (code == none)
			return "none";

		std::string buttons;
		if (code & left) buttons += "left, ";
		if (code & right) buttons += "right, ";
		if (code & middle) buttons += "middle, ";
		if (code & button4) buttons += "4, ";
		if (code & button5) buttons += "5, ";
		if (code & button6) buttons += "6, ";
		if (code & button7) buttons += "7, ";
		if (code & button8) buttons += "8, ";

		buttons.pop_back();
		buttons.pop_back();
		return buttons;
	}

	MYL_NO_DISCARD constexpr std::string key_to_string(myl::key_code code) {
		switch (code) {
			using namespace myl::key;
			case n0: return "0";
			case n1: return "1";
			case n2: return "2";
			case n3: return "3";
			case n4: return "4";
			case n5: return "5";
			case n6: return "6";
			case n7: return "7";
			case n8: return "8";
			case n9: return "9";
			TB_CODE_TO_STRING_CASE(a);
			TB_CODE_TO_STRING_CASE(b);
			TB_CODE_TO_STRING_CASE(c);
			TB_CODE_TO_STRING_CASE(d);
			TB_CODE_TO_STRING_CASE(e);
			TB_CODE_TO_STRING_CASE(f);
			TB_CODE_TO_STRING_CASE(g);
			TB_CODE_TO_STRING_CASE(h);
			TB_CODE_TO_STRING_CASE(i);
			TB_CODE_TO_STRING_CASE(j);
			TB_CODE_TO_STRING_CASE(k);
			TB_CODE_TO_STRING_CASE(l);
			TB_CODE_TO_STRING_CASE(m);
			TB_CODE_TO_STRING_CASE(n);
			TB_CODE_TO_STRING_CASE(o);
			TB_CODE_TO_STRING_CASE(p);
			TB_CODE_TO_STRING_CASE(q);
			TB_CODE_TO_STRING_CASE(r);
			TB_CODE_TO_STRING_CASE(s);
			TB_CODE_TO_STRING_CASE(t);
			TB_CODE_TO_STRING_CASE(u);
			TB_CODE_TO_STRING_CASE(v);
			TB_CODE_TO_STRING_CASE(w);
			TB_CODE_TO_STRING_CASE(x);
			TB_CODE_TO_STRING_CASE(y);
			TB_CODE_TO_STRING_CASE(z);
			case kp0: return "keypad 0";
			case kp1: return "keypad 1";
			case kp2: return "keypad 2";
			case kp3: return "keypad 3";
			case kp4: return "keypad 4";
			case kp5: return "keypad 5";
			case kp6: return "keypad 6";
			case kp7: return "keypad 7";
			case kp8: return "keypad 8";
			case kp9: return "keypad 9";
			TB_CODE_TO_STRING_CASE(decimal);
			TB_CODE_TO_STRING_CASE(divide);
			TB_CODE_TO_STRING_CASE(multiply);
			TB_CODE_TO_STRING_CASE(subtract);
			TB_CODE_TO_STRING_CASE(add);
			TB_CODE_TO_STRING_CASE(space);
			TB_CODE_TO_STRING_CASE(apostrophe);
			TB_CODE_TO_STRING_CASE(comma);
			TB_CODE_TO_STRING_CASE(dash);
			TB_CODE_TO_STRING_CASE(period);
			TB_CODE_TO_STRING_CASE(slash);
			TB_CODE_TO_STRING_CASE(semicolon);
			TB_CODE_TO_STRING_CASE(equal);
			TB_CODE_TO_STRING_CASE(left_bracket);
			TB_CODE_TO_STRING_CASE(right_bracket);
			TB_CODE_TO_STRING_CASE(backslash);
			TB_CODE_TO_STRING_CASE(grave_accent);
			TB_CODE_TO_STRING_CASE(backspace);
			TB_CODE_TO_STRING_CASE(tab);
			TB_CODE_TO_STRING_CASE(enter);
			TB_CODE_TO_STRING_CASE(right);
			TB_CODE_TO_STRING_CASE(left);
			TB_CODE_TO_STRING_CASE(down);
			TB_CODE_TO_STRING_CASE(up);
			TB_CODE_TO_STRING_CASE(escape);
			TB_CODE_TO_STRING_CASE(insert);
			TB_CODE_TO_STRING_CASE(delete_key);
			TB_CODE_TO_STRING_CASE(page_up);
			TB_CODE_TO_STRING_CASE(page_down);
			TB_CODE_TO_STRING_CASE(home);
			TB_CODE_TO_STRING_CASE(end);
			TB_CODE_TO_STRING_CASE(world1);
			TB_CODE_TO_STRING_CASE(world2);
			TB_CODE_TO_STRING_CASE(caps_lock);
			TB_CODE_TO_STRING_CASE(scroll_lock);
			TB_CODE_TO_STRING_CASE(num_lock);
			TB_CODE_TO_STRING_CASE(print_screen);
			TB_CODE_TO_STRING_CASE(pause);
			TB_CODE_TO_STRING_CASE(f1);
			TB_CODE_TO_STRING_CASE(f2);
			TB_CODE_TO_STRING_CASE(f3);
			TB_CODE_TO_STRING_CASE(f4);
			TB_CODE_TO_STRING_CASE(f5);
			TB_CODE_TO_STRING_CASE(f6);
			TB_CODE_TO_STRING_CASE(f7);
			TB_CODE_TO_STRING_CASE(f8);
			TB_CODE_TO_STRING_CASE(f9);
			TB_CODE_TO_STRING_CASE(f10);
			TB_CODE_TO_STRING_CASE(f11);
			TB_CODE_TO_STRING_CASE(f12);
			TB_CODE_TO_STRING_CASE(f13);
			TB_CODE_TO_STRING_CASE(f14);
			TB_CODE_TO_STRING_CASE(f15);
			TB_CODE_TO_STRING_CASE(f16);
			TB_CODE_TO_STRING_CASE(f17);
			TB_CODE_TO_STRING_CASE(f18);
			TB_CODE_TO_STRING_CASE(f19);
			TB_CODE_TO_STRING_CASE(f20);
			TB_CODE_TO_STRING_CASE(f21);
			TB_CODE_TO_STRING_CASE(f22);
			TB_CODE_TO_STRING_CASE(f23);
			TB_CODE_TO_STRING_CASE(f24);
			TB_CODE_TO_STRING_CASE(left_shift);
			TB_CODE_TO_STRING_CASE(left_control);
			TB_CODE_TO_STRING_CASE(left_alt);
			TB_CODE_TO_STRING_CASE(left_super);
			TB_CODE_TO_STRING_CASE(right_shift);
			TB_CODE_TO_STRING_CASE(right_control);
			TB_CODE_TO_STRING_CASE(right_alt);
			TB_CODE_TO_STRING_CASE(right_super);
			default: return "unknown";
		}
	}
}
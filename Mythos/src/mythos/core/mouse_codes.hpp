#pragma once
#include <mythos/defines.hpp>

namespace myl {
	using mouse_code = u16;
	namespace mouse_button {
		enum : mouse_code {
			button1,
			button2,
			button3,
			button4,
			button5,
			button6,
			button7,
			button8,

			size,
			unknown,

			left = button1,
			right = button2,
			middle = button3
		};
	}
}
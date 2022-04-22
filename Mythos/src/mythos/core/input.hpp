#pragma once
#include "key_codes.hpp"
#include "mouse_codes.hpp"

/// MYBug: left off of kohi 9 part 1 at 11:44/27:00

namespace myl::input {
	enum class input_state {
		up,
		down,
		held
	};

	struct states {
		static input_state s_previous_key_states[key::size]; /// MYTodo: is this the right way to do this?
		static input_state s_key_states[key::size];
		static input_state s_previous_mouse_button_states[mouse_button::size];
		static input_state s_mouse_button_states[mouse_button::size];

		static void init();
		static void update();
	};

	bool platform_key_down(key_code); /// MYTodo: Implement the platform code

	MYL_API input_state get_key_state(key_code);
	MYL_API input_state get_previous_key_state(key_code);
	MYL_API bool key_down(key_code);
	MYL_API bool key_up(key_code);
	MYL_API bool key_clicked(key_code);
	MYL_API bool key_released(key_code);

	bool platform_mouse_button_down(mouse_code); /// MYTodo: Implement the platform code

	MYL_API input_state get_mouse_button_state(mouse_code);
	MYL_API input_state get_previous_mouse_button_state(mouse_code);
	MYL_API bool mouse_button_down(mouse_code);
	MYL_API bool mouse_button_up(mouse_code);
	MYL_API bool mouse_button_clicked(mouse_code);
	MYL_API bool mouse_button_released(mouse_code);

	MYL_API ivec2 cursor_position(); /// MYTodo: platform code
	MYL_API ivec2 previous_cursor_position(); /// MYTodo: platform code
	MYL_API ivec2 mouse_wheel_delta(); /// MYTodo: platform code
}
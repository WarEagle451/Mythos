#pragma once
#include "key_codes.hpp"
#include "mouse_codes.hpp"

#include <mythos/math/vec2.hpp>

/// MYTodo: Should this really be in core?

/// MYTodo: key mods

namespace myl::input {
	enum class state { /// MYTodo: this could probs be replaced with bits
		up,
		down
	};

	struct internal_states {
		static state s_previous_key_states[key::size];
		static state s_key_states[key::size];
		static state s_previous_mouse_button_states[mouse_button::size];
		static state s_mouse_button_states[mouse_button::size];
		static f32vec2 s_previous_cursor_position;
		static f32vec2 s_cursor_position;

		static void init();
		static void update();
	};

	//@brief called by the platform to update a key state
	void process_key(key_code, state);
	//@brief called by the platform to update a mouse button state
	void process_mouse_button(mouse_code, state);
	//@brief called by the platform to update cursor position
	void process_cursor_position(f32 a_x, f32 a_y);
	void process_mouse_wheel(const f32vec2&);

	MYL_API MYL_NO_DISCARD state get_key_state(key_code);
	MYL_API MYL_NO_DISCARD state get_previous_key_state(key_code);
	MYL_API bool key_down(key_code);
	MYL_API bool key_up(key_code);
	MYL_API bool key_clicked(key_code);
	MYL_API bool key_released(key_code);

	MYL_API MYL_NO_DISCARD state get_mouse_button_state(mouse_code);
	MYL_API MYL_NO_DISCARD state get_previous_mouse_button_state(mouse_code);
	MYL_API bool mouse_button_down(mouse_code);
	MYL_API bool mouse_button_up(mouse_code);
	MYL_API bool mouse_button_clicked(mouse_code);
	MYL_API bool mouse_button_released(mouse_code);

	MYL_API f32vec2 MYL_NO_DISCARD cursor_position();
	MYL_API f32vec2 MYL_NO_DISCARD previous_cursor_position();
	MYL_API f32vec2 MYL_NO_DISCARD cursor_delta();
}
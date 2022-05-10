#pragma once
#include "key_codes.hpp"
#include "mouse_codes.hpp"

#include <mythos/math/vec2.hpp>

/// MYTodo: Should this really be in core?
/// MYTodo: Key mods

namespace myl::input {
	enum class state { /// MYTodo: This could probs be replaced with bits
		up,
		down
	};

	struct internal_states {
		static state s_previous_key_states[key::size];
		static state s_key_states[key::size];
		static mouse_code s_previous_mouse_button_states;
		static mouse_code s_mouse_button_states;
		static f32vec2 s_previous_cursor_position;
		static f32vec2 s_cursor_position;

		static void init();
		static void update();
	};

	//@brief Called by the platform to update a key state
	void process_key(key_code, state, u32);
	//@brief Called by the platform to update mouses button state. Can handle multiple buttons at once
	void process_mouse_buttons(mouse_code, state);
	//@brief Called by the platform to update cursor position
	void process_cursor_position(const f32vec2&);
	void process_mouse_wheel(const f32vec2&);

	MYL_API MYL_NO_DISCARD state key_state(key_code);
	MYL_API MYL_NO_DISCARD state previous_key_state(key_code);
	MYL_API MYL_NO_DISCARD bool key_down(key_code);
	MYL_API MYL_NO_DISCARD bool key_up(key_code);
	//@brief Keys like shift, control and alt don't hold states
	MYL_API MYL_NO_DISCARD bool key_pressed(key_code);
	//@brief Keys like shift, control and alt don't hold states
	MYL_API MYL_NO_DISCARD bool key_released(key_code);

	//@brief Capable of checking multiple buttons at once
	MYL_API MYL_NO_DISCARD state mouse_button_state(mouse_code);
	//@brief Capable of checking multiple buttons at once
	MYL_API MYL_NO_DISCARD state previous_mouse_button_state(mouse_code);
	//@brief Capable of checking multiple buttons at once
	MYL_API MYL_NO_DISCARD bool mouse_button_down(mouse_code);
	//@brief Capable of checking multiple buttons at once
	MYL_API MYL_NO_DISCARD bool mouse_button_up(mouse_code);
	//@brief Capable of checking multiple buttons at once
	MYL_API MYL_NO_DISCARD bool mouse_button_pressed(mouse_code);
	//@brief Capable of checking multiple buttons at once
	MYL_API MYL_NO_DISCARD bool mouse_button_released(mouse_code);

	MYL_API MYL_NO_DISCARD f32vec2 cursor_position();
	MYL_API MYL_NO_DISCARD f32vec2 previous_cursor_position();
	MYL_API MYL_NO_DISCARD f32vec2 cursor_delta();
}
#pragma once
#include "core/key_codes.hpp"
#include "core/mouse_codes.hpp"

#include <mythos/math/vec2.hpp>

/// MYTodo: Key mods
/// MYTodo: Track toggled keys

namespace myl::input {
	enum class state { up, down };

	void init();
	void update();

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
	MYL_API MYL_NO_DISCARD bool key_pressed(key_code);
	MYL_API MYL_NO_DISCARD bool key_released(key_code);

	//@brief Capable of checking multiple buttons at once.
	//@return state::up if all buttons are not down, otherwise state::down
	MYL_API MYL_NO_DISCARD state mouse_button_state(mouse_code);
	//@brief Capable of checking multiple buttons at once.
	//@return state::up if all buttons are not down, otherwise state::down
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
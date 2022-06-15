#pragma once
#include "core/key_codes.hpp"
#include "core/mouse_codes.hpp"

#include <mythos/math/vec2.hpp>

/// MYTodo: Key mods
/// MYTodo: Track toggled keys

namespace myl::input { /// MYTodo: Should this be a class?
	enum class state { up, down };

	void init();
	void update();

	void process_key(key_code, state, u32);

	void process_mouse_buttons_up(mouse_code);
	void process_mouse_buttons_down(mouse_code);
	void process_mouse_buttons(mouse_code, state);

	void process_cursor_delta(const f32vec2&);
	void process_cursor_delta_given_absolute(const f32vec2&);
	void process_window_cursor_position(const f32vec2&);
	void process_mouse_wheel(const f32vec2&);

	MYL_API MYL_NO_DISCARD state key_state(key_code);
	MYL_API MYL_NO_DISCARD state previous_key_state(key_code);
	MYL_API MYL_NO_DISCARD bool key_down(key_code);
	MYL_API MYL_NO_DISCARD bool key_up(key_code);
	MYL_API MYL_NO_DISCARD bool key_pressed(key_code);
	MYL_API MYL_NO_DISCARD bool key_released(key_code);

	//@return state::up if all buttons are not down, otherwise state::down
	MYL_API MYL_NO_DISCARD state mouse_button_state(mouse_code);
	//@return state::up if all buttons are not down, otherwise state::down
	MYL_API MYL_NO_DISCARD state previous_mouse_button_state(mouse_code);
	MYL_API MYL_NO_DISCARD bool mouse_button_down(mouse_code);
	MYL_API MYL_NO_DISCARD bool mouse_button_up(mouse_code);
	MYL_API MYL_NO_DISCARD bool mouse_button_pressed(mouse_code);
	MYL_API MYL_NO_DISCARD bool mouse_button_released(mouse_code);

	MYL_API MYL_NO_DISCARD const f32vec2& cursor_position();
	MYL_API MYL_NO_DISCARD const f32vec2& previous_cursor_position();
	MYL_API MYL_NO_DISCARD const f32vec2& cursor_delta();
	MYL_API MYL_NO_DISCARD const f32vec2& previous_cursor_delta();
}
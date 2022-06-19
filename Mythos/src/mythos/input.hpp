#pragma once
#include "core/key_codes.hpp"
#include "core/mouse_codes.hpp"

#include <mythos/math/vec2.hpp>

/// MYTodo: Key mods
/// MYTodo: Track toggled keys
/// MYTodo: Key remapping

namespace myl {
	class input {
	public:
		enum class state : bool { up, down };
	private:
		static state s_previous_key_states[key::size];
		static state s_key_states[key::size];
		static mouse_code s_previous_mouse_button_states;
		static mouse_code s_mouse_button_states;
		static f32vec2 s_previous_window_cursor_position;
		static f32vec2 s_window_cursor_position;
		static f32vec2 s_mouse_delta;
		static f32vec2 s_previous_mouse_delta;
	public:
		static void init();
		static void update();

		static void process_key(key_code, state, u32 = 0); /// MYTodo: Should key_repeat be removed
		static void process_key_typed(char, key_code a_mods);

		static void process_mouse_buttons_up(mouse_code);
		static void process_mouse_buttons_down(mouse_code);
		static void process_mouse_buttons(mouse_code, state);

		static void process_cursor_delta(const f32vec2&);
		static void process_cursor_delta_given_absolute(const f32vec2&);
		static void process_window_cursor_position(const f32vec2&);
		static void process_mouse_wheel(const f32vec2&);

		MYL_API MYL_NO_DISCARD static state previous_key_state(key_code);
		MYL_API MYL_NO_DISCARD static state key_state(key_code);
		MYL_API MYL_NO_DISCARD static bool key_up(key_code);
		MYL_API MYL_NO_DISCARD static bool key_down(key_code);
		MYL_API MYL_NO_DISCARD static bool key_released(key_code);
		MYL_API MYL_NO_DISCARD static bool key_pressed(key_code);

		//@return state::up if all buttons are not down, otherwise state::down
		MYL_API MYL_NO_DISCARD static state previous_mouse_button_state(mouse_code);
		//@return state::up if all buttons are not down, otherwise state::down
		MYL_API MYL_NO_DISCARD static state mouse_button_state(mouse_code);
		MYL_API MYL_NO_DISCARD static bool mouse_button_up(mouse_code);
		MYL_API MYL_NO_DISCARD static bool mouse_button_down(mouse_code);
		MYL_API MYL_NO_DISCARD static bool mouse_button_released(mouse_code);
		MYL_API MYL_NO_DISCARD static bool mouse_button_pressed(mouse_code);

		MYL_API MYL_NO_DISCARD static const f32vec2& previous_window_cursor_position();
		MYL_API MYL_NO_DISCARD static const f32vec2& window_cursor_position();
		MYL_API MYL_NO_DISCARD static const f32vec2& previous_cursor_delta();
		MYL_API MYL_NO_DISCARD static const f32vec2& cursor_delta();

		/// MYTodo: fake trigger button clicks, move mouse and key clicks
	};
}
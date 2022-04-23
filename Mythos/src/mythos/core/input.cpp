#include "input.hpp"

#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>

namespace myl::input {
	state internal_states::s_previous_key_states[key::size];
	state internal_states::s_key_states[key::size];
	state internal_states::s_previous_mouse_button_states[mouse_button::size];
	state internal_states::s_mouse_button_states[mouse_button::size];
	fvec2 internal_states::s_previous_cursor_position;
	fvec2 internal_states::s_cursor_position;

	void internal_states::init() {
		s_previous_cursor_position = { 0, 0 };
		s_cursor_position = { 0, 0 };

		for (u32 i = 0; i != key::size; ++i) {
			s_previous_key_states[i] = state::up;
			s_key_states[i] = state::up;
		}

		for (u32 i = 0; i != mouse_button::size; ++i) {
			s_previous_mouse_button_states[i] = state::up;
			s_mouse_button_states[i] = state::up;
		}
	}

	void internal_states::update() {
		*s_previous_key_states = *s_key_states;
		*s_previous_mouse_button_states = *s_mouse_button_states;
		s_previous_cursor_position = s_cursor_position;
	}

	void process_key(key_code a_code, state a_state) {
		if (internal_states::s_key_states[a_code] != a_state) { // only update when they change state
			internal_states::s_key_states[a_code] = a_state;

			// fire event to process it
			if (a_state == state::up) {
				key_released_event e(a_code);
				fire_event(e);
			}
			else {
				key_pressed_event e(a_code, 0); /// MYTodo: repeat count;
				fire_event(e);
			}
		}
	}

	void process_mouse_button(mouse_code a_code, state a_state) {
		if (internal_states::s_mouse_button_states[a_code] != a_state) { // only update when they change state
			internal_states::s_mouse_button_states[a_code] = a_state;

			// fire event to process it
			if (a_state == state::up) {
				mouse_released_event e(a_code);
				fire_event(e);
			}
			else {
				mouse_pressed_event e(a_code);
				fire_event(e);
			}
		}
	}

	void process_cursor_position(f32 a_x, f32 a_y) {
		if (internal_states::s_cursor_position.x != a_x || internal_states::s_cursor_position.y != a_y) {  // only update when they change state
			internal_states::s_cursor_position = { a_x, a_y };

			mouse_moved_event e(a_x, a_y);
			fire_event(e);
		}
	}

	void process_mouse_wheel(const fvec2& a_scroll_delta) {
		mouse_scrolled_event e(a_scroll_delta.x, a_scroll_delta.y);
		fire_event(e);
	}

	state get_key_state(key_code a_code) {
		return internal_states::s_key_states[a_code];
	}

	state get_previous_key_state(key_code a_code) {
		return internal_states::s_previous_key_states[a_code];
	}

	bool key_down(key_code a_code) {
		return internal_states::s_key_states[a_code] == state::down;
	}

	bool key_up(key_code a_code) {
		return internal_states::s_key_states[a_code] == state::up;
	}

	bool key_clicked(key_code a_code) {
		return
			internal_states::s_previous_key_states[a_code] == state::up && /// MYBug: why does this seem to only detect when its pressed down ; maybe have to send on key typed event in process stuff
			internal_states::s_key_states[a_code] == state::down; /// this works right in kohi 10 @ ~ 17:00
	}

	bool key_released(key_code a_code) {
		return
			internal_states::s_previous_key_states[a_code] == state::down &&
			internal_states::s_key_states[a_code] == state::up;
	}

	state get_mouse_button_state(mouse_code a_code) {
		return internal_states::s_mouse_button_states[a_code];
	}

	state get_previous_mouse_button_state(mouse_code a_code) {
		return internal_states::s_previous_mouse_button_states[a_code];
	}

	bool mouse_button_down(mouse_code a_code) {
		return internal_states::s_mouse_button_states[a_code] == state::down;
	}

	bool mouse_button_up(mouse_code a_code) {
		return internal_states::s_mouse_button_states[a_code] == state::up;
	}

	bool mouse_button_clicked(mouse_code a_code) {
		return
			internal_states::s_previous_mouse_button_states[a_code] == state::up &&
			internal_states::s_mouse_button_states[a_code] == state::down;
	}

	bool mouse_button_released(mouse_code a_code) {
		return
			internal_states::s_previous_mouse_button_states[a_code] == state::down &&
			internal_states::s_mouse_button_states[a_code] == state::up;
	}

	fvec2 cursor_position() {
		return internal_states::s_cursor_position;
	}

	fvec2 previous_cursor_position() {
		return internal_states::s_previous_cursor_position;
	}

	fvec2 cursor_delta() {
		return internal_states::s_cursor_position - internal_states::s_previous_cursor_position;
	}
}
#include "input.hpp"

#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>

namespace myl::input {
	struct internal_states {
		static state s_previous_key_states[key::size];
		static state s_key_states[key::size];
		static mouse_code s_previous_mouse_button_states;
		static mouse_code s_mouse_button_states;
		static f32vec2 s_previous_cursor_position;
		static f32vec2 s_cursor_position;
	};

	state internal_states::s_previous_key_states[key::size];
	state internal_states::s_key_states[key::size];
	mouse_code internal_states::s_previous_mouse_button_states;
	mouse_code internal_states::s_mouse_button_states;
	f32vec2 internal_states::s_previous_cursor_position;
	f32vec2 internal_states::s_cursor_position;

	void init() {
		internal_states::s_previous_cursor_position = { 0, 0 };
		internal_states::s_cursor_position = { 0, 0 };

		for (u32 i = 0; i != key::size; ++i) {
			internal_states::s_previous_key_states[i] = state::up;
			internal_states::s_key_states[i] = state::up;
		}

		internal_states::s_previous_mouse_button_states = mouse_button::none;
		internal_states::s_mouse_button_states = mouse_button::none;
	}

	/// MYBug: If the window loses focus and a key or mouse button is down when the window regains focus it will stay down until the key or button event up happens
	/// On refocus maybe query the platform to update all input states
	void update() {
		*internal_states::s_previous_key_states = *internal_states::s_key_states;
		internal_states::s_previous_mouse_button_states = internal_states::s_mouse_button_states;
		internal_states::s_previous_cursor_position = internal_states::s_cursor_position;
	}

	void process_key(key_code a_code, state a_state, u32 a_repeat_count) {
		if (a_code == key::unknown)
			return;

		if (internal_states::s_key_states[a_code] != a_state) { // Only update when they change state
			internal_states::s_key_states[a_code] = a_state;

			if (a_state == state::up) {
				event_key_released e(a_code);
				fire_event(e);
			}
			else {
				event_key_pressed e(a_code, 0);
				fire_event(e);
			}
		}
		else if (a_state == state::down) { // Key repeats
			event_key_pressed e(a_code, a_repeat_count); /// MYTodo: Might have to keep track of key repeats internally as windows only ever returns 1
			fire_event(e);
		}
	}

	void process_mouse_buttons(mouse_code a_code, state a_state) {
		// Bit changes from 1 to 0 don't matter beacuse it's a bit that's changed for the opposite state
		// Platform should fire an event for that state change where it will be handled here again

		if (a_state == state::up) {
			// new = 0110; old = 1010
			// old & new = 0010. Therefor bit 2 has changed state to up
			mouse_code changed_buttons = internal_states::s_mouse_button_states & a_code;
			if (changed_buttons != 0) { // Only update if there is a change
				internal_states::s_mouse_button_states &= ~changed_buttons; // Clear bits to up
				event_mouse_released e(changed_buttons);
				fire_event(e);
			}
		}
		else { // a_state == state::down
			// new = 0110; old = 1010
			// ~(old | ~new) = 0100. Therefor bit 3 has changed state to down
			mouse_code changed_buttons = ~(internal_states::s_mouse_button_states | ~a_code); // Getting changed down buttons
			if (changed_buttons != 0) { // Only update if there is a change
				internal_states::s_mouse_button_states |= changed_buttons; // Set bits to down
				event_mouse_pressed e(changed_buttons);
				fire_event(e);
			}
		}
	}

	void process_cursor_position(const f32vec2& position) {
		if (internal_states::s_cursor_position.x != position.x || internal_states::s_cursor_position.y != position.y) {  // Only update when they change state
			internal_states::s_cursor_position = position;

			event_mouse_moved e(position);
			fire_event(e);
		}
	}

	void process_mouse_wheel(const f32vec2& a_scroll_delta) {
		event_mouse_scrolled e(a_scroll_delta);
		fire_event(e);
	}

	state key_state(key_code a_code) {
		return internal_states::s_key_states[a_code];
	}

	state previous_key_state(key_code a_code) {
		return internal_states::s_previous_key_states[a_code];
	}

	bool key_down(key_code a_code) {
		return internal_states::s_key_states[a_code] == state::down;
	}

	bool key_up(key_code a_code) {
		return internal_states::s_key_states[a_code] == state::up;
	}

	bool key_pressed(key_code a_code) {
		return
			internal_states::s_previous_key_states[a_code] == state::up &&
			internal_states::s_key_states[a_code] == state::down;
	}

	bool key_released(key_code a_code) {
		return
			internal_states::s_previous_key_states[a_code] == state::down &&
			internal_states::s_key_states[a_code] == state::up;
	}

	state mouse_button_state(mouse_code a_code) {
		return (internal_states::s_mouse_button_states & a_code) == a_code ? state::down : state::up;
	}

	state previous_mouse_button_state(mouse_code a_code) {
		return (internal_states::s_previous_mouse_button_states & a_code) == a_code ? state::down : state::up;
	}

	bool mouse_button_down(mouse_code a_code) {
		return (internal_states::s_mouse_button_states & a_code) == a_code;
	}

	bool mouse_button_up(mouse_code a_code) {
		return (~internal_states::s_mouse_button_states & a_code) == a_code;
	}

	bool mouse_button_pressed(mouse_code a_code) {
		return
			(~internal_states::s_previous_mouse_button_states & a_code) == a_code && // up
			(internal_states::s_mouse_button_states & a_code) == a_code; // down
	}

	bool mouse_button_released(mouse_code a_code) {
		return
			(internal_states::s_previous_mouse_button_states & a_code) == a_code && // down
			(~internal_states::s_mouse_button_states & a_code) == a_code; // up
	}

	f32vec2 cursor_position() {
		return internal_states::s_cursor_position;
	}

	f32vec2 previous_cursor_position() {
		return internal_states::s_previous_cursor_position;
	}

	f32vec2 cursor_delta() {
		return internal_states::s_cursor_position - internal_states::s_previous_cursor_position;
	}
}
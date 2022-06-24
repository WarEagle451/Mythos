#include "input.hpp"

#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>

namespace myl {
	input::state input::s_previous_key_states[key::size];
	input::state input::s_key_states[key::size];
	mouse_code input::s_previous_mouse_button_states;
	mouse_code input::s_mouse_button_states;
	f32vec2 input::s_previous_window_cursor_position;
	f32vec2 input::s_window_cursor_position;
	f32vec2 input::s_mouse_delta;
	f32vec2 input::s_previous_mouse_delta;

	void input::init() {
		s_previous_window_cursor_position = { 0, 0 };
		s_window_cursor_position = { 0, 0 };
		s_mouse_delta = { 0, 0 };
		s_previous_mouse_delta = { 0, 0 };

		for (u32 i = 0; i != key::size; ++i) {
			s_previous_key_states[i] = state::up;
			s_key_states[i] = state::up;
		}

		s_previous_mouse_button_states = mouse_button::none;
		s_mouse_button_states = mouse_button::none;
	}

	/// MYBug: If the window loses focus and a key or mouse button is down when the window regains focus it will stay down until the key or button event up happens
	/// MYTodo: When a window loses focus clear keys
	/// On refocus maybe query the platform to update all input states
	void input::update() {
		*s_previous_key_states = *s_key_states;
		s_previous_mouse_button_states = s_mouse_button_states;
		s_previous_window_cursor_position = s_window_cursor_position;
		s_previous_mouse_delta = s_mouse_delta;
		s_mouse_delta = { 0, 0 };
	}

	void input::process_key(key_code a_code, state a_state) {
		if (a_code == key::unknown)
			return;

		// This might only work because every frame the keystate is copied over, but is not cleared from the current states
		if (s_key_states[a_code] != a_state) { // Only update when they change state
			s_key_states[a_code] = a_state;

			if (a_state == state::up) {
				event_key_released e(a_code);
				fire_event(e);
			}
			else {
				event_key_pressed e(a_code, false);
				fire_event(e);
			}
		}
		else if (a_state == state::down) { // Key repeats
			event_key_pressed e(a_code, true);
			fire_event(e);
		}
	}

	void input::process_key_typed(u32 a_char) {
		if (a_char < 32 || (a_char > 126 && a_char < 160)) /// MYTodo: Do this better
			return;
		event_key_typed e(a_char);
		fire_event(e);
	}

	void input::process_mouse_buttons_up(mouse_code a_code) {
		// new = 0110; old = 1010
		// old & new = 0010. Therefor bit 2 has changed state to up
		mouse_code changed_buttons = s_mouse_button_states & a_code;
		if (changed_buttons != 0) { // Only update if there is a change
		s_mouse_button_states &= ~changed_buttons; // Clear bits to up
			event_mouse_released e(changed_buttons);
			fire_event(e);
		}
	}

	void input::process_mouse_buttons_down(mouse_code a_code) {
		// new = 0110; old = 1010
		// ~(old | ~new) = 0100. Therefor bit 3 has changed state to down
		mouse_code changed_buttons = ~(s_mouse_button_states | ~a_code); // Getting changed down buttons
		if (changed_buttons != 0) { // Only update if there is a change
			s_mouse_button_states |= changed_buttons; // Set bits to down
			event_mouse_pressed e(changed_buttons);
			fire_event(e);
		}
	}

	void input::process_mouse_buttons(mouse_code a_code, state a_state) {
		a_state == state::up ? process_mouse_buttons_up(a_code) : process_mouse_buttons_down(a_code);
	}

	void input::process_cursor_delta(const f32vec2& a_delta) {
		if (a_delta.x != 0.f || a_delta.y != 0.f) // No event is sent because this is to be queried
			s_mouse_delta = a_delta;
	}

	void input::process_cursor_delta_given_absolute(const f32vec2& a_absolute) {
		/// MYTodo: Figure out how to deal with this
	}

	void input::process_window_cursor_position(const f32vec2& a_position) {
		if (s_window_cursor_position.x != a_position.x || s_window_cursor_position.y != a_position.y) {  // Only update when they change state
			s_window_cursor_position = a_position;
			event_mouse_moved e(s_window_cursor_position);
			fire_event(e);
		}
	}

	void input::process_mouse_wheel(const f32vec2& a_scroll_delta) {
		event_mouse_scrolled e(a_scroll_delta);
		fire_event(e);
	}

	input::state input::previous_key_state(key_code a_code) {
		return s_previous_key_states[a_code];
	}

	input::state input::key_state(key_code a_code) {
		return s_key_states[a_code];
	}

	bool input::key_up(key_code a_code) {
		return s_key_states[a_code] == state::up;
	}

	bool input::key_down(key_code a_code) {
		return s_key_states[a_code] == state::down;
	}

	bool input::key_released(key_code a_code) {
		return
			s_previous_key_states[a_code] == state::down &&
			s_key_states[a_code] == state::up;
	}

	bool input::key_pressed(key_code a_code) {
		return
			s_previous_key_states[a_code] == state::up &&
			s_key_states[a_code] == state::down;
	}

	input::state input::previous_mouse_button_state(mouse_code a_code) {
		return (s_previous_mouse_button_states & a_code) == a_code ? state::down : state::up;
	}

	input::state input::mouse_button_state(mouse_code a_code) {
		return (s_mouse_button_states & a_code) == a_code ? state::down : state::up;
	}

	bool input::mouse_button_up(mouse_code a_code) {
		return (~s_mouse_button_states & a_code) == a_code;
	}

	bool input::mouse_button_down(mouse_code a_code) {
		return (s_mouse_button_states & a_code) == a_code;
	}

	bool input::mouse_button_released(mouse_code a_code) {
		return
			(s_previous_mouse_button_states & a_code) == a_code && // down
			(~s_mouse_button_states & a_code) == a_code; // up
	}

	bool input::mouse_button_pressed(mouse_code a_code) {
		return
			(~s_previous_mouse_button_states & a_code) == a_code && // up
			(s_mouse_button_states & a_code) == a_code; // down
	}

	const f32vec2& input::previous_window_cursor_position() {
		return s_previous_window_cursor_position;
	}

	const f32vec2& input::window_cursor_position() {
		return s_window_cursor_position;
	}

	const f32vec2& input::previous_cursor_delta() {
		return s_previous_mouse_delta;
	}

	const f32vec2& input::cursor_delta() {
		return s_mouse_delta;
	}
}
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
		static f32vec2 s_mouse_delta;
		static f32vec2 s_previous_mouse_delta;
	};

	state internal_states::s_previous_key_states[key::size];
	state internal_states::s_key_states[key::size];
	mouse_code internal_states::s_previous_mouse_button_states;
	mouse_code internal_states::s_mouse_button_states;
	f32vec2 internal_states::s_previous_cursor_position;
	f32vec2 internal_states::s_cursor_position;
	f32vec2 internal_states::s_mouse_delta;
	f32vec2 internal_states::s_previous_mouse_delta;

	void init() {
		internal_states::s_previous_cursor_position = { 0, 0 };
		internal_states::s_cursor_position = { 0, 0 };
		internal_states::s_mouse_delta = { 0, 0 };
		internal_states::s_previous_mouse_delta = { 0, 0 };

		for (u32 i = 0; i != key::size; ++i) {
			internal_states::s_previous_key_states[i] = state::up;
			internal_states::s_key_states[i] = state::up;
		}

		internal_states::s_previous_mouse_button_states = mouse_button::none;
		internal_states::s_mouse_button_states = mouse_button::none;
	}

	/// MYBug: If the window loses focus and a key or mouse button is down when the window regains focus it will stay down until the key or button event up happens
	/// On refocus maybe query the platform to update all input states
	void update() { /// MYTodo: Could probs make better
		*internal_states::s_previous_key_states = *internal_states::s_key_states;
		internal_states::s_previous_mouse_button_states = internal_states::s_mouse_button_states;
		internal_states::s_previous_cursor_position = internal_states::s_cursor_position;
		internal_states::s_previous_mouse_delta = internal_states::s_mouse_delta;
		internal_states::s_mouse_delta = { 0, 0 };
	}

	void process_key(key_code code, state state, u32 repeat_count) {
		if (code == key::unknown)
			return;

		if (internal_states::s_key_states[code] != state) { // Only update when they change state
			internal_states::s_key_states[code] = state;

			if (state == state::up) {
				event_key_released e(code);
				fire_event(e);
			}
			else {
				event_key_pressed e(code, 0);
				fire_event(e);
			}
		}
		else if (state == state::down) { // Key repeats
			event_key_pressed e(code, repeat_count); /// MYTodo: Might have to keep track of key repeats internally as windows only ever returns 1
			fire_event(e);
		}
	}

	void process_mouse_buttons_up(mouse_code code) {
		// new = 0110; old = 1010
		// old & new = 0010. Therefor bit 2 has changed state to up
		mouse_code changed_buttons = internal_states::s_mouse_button_states & code;
		if (changed_buttons != 0) { // Only update if there is a change
			internal_states::s_mouse_button_states &= ~changed_buttons; // Clear bits to up
			event_mouse_released e(changed_buttons);
			fire_event(e);
		}
	}

	void process_mouse_buttons_down(mouse_code code) {
		// new = 0110; old = 1010
		// ~(old | ~new) = 0100. Therefor bit 3 has changed state to down
		mouse_code changed_buttons = ~(internal_states::s_mouse_button_states | ~code); // Getting changed down buttons
		if (changed_buttons != 0) { // Only update if there is a change
			internal_states::s_mouse_button_states |= changed_buttons; // Set bits to down
			event_mouse_pressed e(changed_buttons);
			fire_event(e);
		}
	}

	void process_mouse_buttons(mouse_code code, state state) {
		state == state::up ? process_mouse_buttons_up(code) : process_mouse_buttons_down(code);
	}

	void process_cursor_delta(const f32vec2& delta) {
		if (delta.x != 0.f || delta.y != 0.f) // No event is sent because this is to be queried
			internal_states::s_mouse_delta = delta;
	}

	void process_cursor_delta_given_absolute(const f32vec2&) {

		/// MYTodo: Figure out how to do this
	}

	void process_window_cursor_position(const f32vec2& position) {
		if (internal_states::s_cursor_position.x != position.x || internal_states::s_cursor_position.y != position.y) {  // Only update when they change state
			internal_states::s_cursor_position = position;
			event_mouse_moved e(internal_states::s_cursor_position);
			fire_event(e);
		}
	}

	void process_mouse_wheel(const f32vec2& scroll_delta) {
		event_mouse_scrolled e(scroll_delta);
		fire_event(e);
	}

	state key_state(key_code code) {
		return internal_states::s_key_states[code];
	}

	state previous_key_state(key_code code) {
		return internal_states::s_previous_key_states[code];
	}

	bool key_down(key_code code) {
		return internal_states::s_key_states[code] == state::down;
	}

	bool key_up(key_code code) {
		return internal_states::s_key_states[code] == state::up;
	}

	bool key_pressed(key_code code) {
		return
			internal_states::s_previous_key_states[code] == state::up &&
			internal_states::s_key_states[code] == state::down;
	}

	bool key_released(key_code code) {
		return
			internal_states::s_previous_key_states[code] == state::down &&
			internal_states::s_key_states[code] == state::up;
	}

	state mouse_button_state(mouse_code code) {
		return (internal_states::s_mouse_button_states & code) == code ? state::down : state::up;
	}

	state previous_mouse_button_state(mouse_code code) {
		return (internal_states::s_previous_mouse_button_states & code) == code ? state::down : state::up;
	}

	bool mouse_button_down(mouse_code code) {
		return (internal_states::s_mouse_button_states & code) == code;
	}

	bool mouse_button_up(mouse_code code) {
		return (~internal_states::s_mouse_button_states & code) == code;
	}

	bool mouse_button_pressed(mouse_code code) {
		return
			(~internal_states::s_previous_mouse_button_states & code) == code && // up
			(internal_states::s_mouse_button_states & code) == code; // down
	}

	bool mouse_button_released(mouse_code code) {
		return
			(internal_states::s_previous_mouse_button_states & code) == code && // down
			(~internal_states::s_mouse_button_states & code) == code; // up
	}

	const f32vec2& cursor_position() {
		return internal_states::s_cursor_position;
	}

	const f32vec2& previous_cursor_position() {
		return internal_states::s_previous_cursor_position;
	}

	const f32vec2& cursor_delta() {
		return internal_states::s_mouse_delta;
	}

	const f32vec2& previous_cursor_delta() {
		return internal_states::s_previous_mouse_delta;
	}
}
#include "input.hpp"

#include <algorithm>

namespace myl::input {
	input_state states::s_previous_key_states[key::size];
	input_state states::s_key_states[key::size];
	input_state states::s_previous_mouse_button_states[mouse_button::size];
	input_state states::s_mouse_button_states[mouse_button::size];

	void states::init() {
		for (u32 i = 0; i != key::size; ++i) {
			s_previous_key_states[i] = input_state::up;
			s_key_states[i] = input_state::up;
		}

		for (u32 i = 0; i != mouse_button::size; ++i) {
			s_previous_mouse_button_states[i] = input_state::up;
			s_mouse_button_states[i] = input_state::up;
		}
	}

	void states::update() {
		*s_previous_key_states = *s_key_states;
		*s_previous_mouse_button_states = *s_mouse_button_states;

		for (u32 i = 0; i != mouse_button::size; ++i) { // updating mouse states
			bool down = platform_is_key_down(i); /// MYTodo: figure out how to get the key state
			s_key_states[i] = down ?
				s_previous_key_states[i] != input_state::up ?
					input_state::held :
					input_state::down :
				input_state::up;
		}

		for (u32 i = 0; i != key::size; ++i) { // updating key states
			bool down = platform_key_down(i);
			s_mouse_button_states[i] = down ?
				s_previous_mouse_button_states[i] != input_state::up ?
					input_state::held :
					input_state::down :
				input_state::up;
		}
	}

	input_state get_key_state(key_code a_code) {
		return states::s_key_states[a_code];
	}

	input_state get_previous_key_state(key_code a_code) {
		return states::s_previous_key_states[a_code];
	}

	bool key_down(key_code a_code) {
		return states::s_key_states[a_code] != input_state::up;
	}

	bool key_up(key_code a_code) {
		return states::s_key_states[a_code] == input_state::up;
	}

	bool key_clicked(key_code a_code) {
		return
			states::s_previous_key_states[a_code] == input_state::up &&
			states::s_key_states[a_code] != input_state::up;
	}

	bool key_released(key_code a_code) {
		return
			states::s_previous_key_states[a_code] != input_state::up &&
			states::s_key_states[a_code] == input_state::up;
	}

	input_state get_mouse_button_state(mouse_code a_code) {
		return states::s_mouse_button_states[a_code];
	}

	input_state get_previous_mouse_button_state(mouse_code a_code) {
		return states::s_previous_mouse_button_states[a_code];
	}

	bool mouse_button_down(mouse_code a_code) {
		return states::s_mouse_button_states[a_code] == input_state::down;
	}

	bool mouse_button_up(mouse_code a_code) {
		return states::s_mouse_button_states[a_code] == input_state::up;
	}

	bool mouse_button_clicked(mouse_code a_code) {
		return
			states::s_previous_mouse_button_states[a_code] == input_state::up &&
			states::s_mouse_button_states[a_code] != input_state::up;
	}

	bool mouse_button_released(mouse_code a_code) {
		return
			states::s_previous_mouse_button_states[a_code] != input_state::up &&
			states::s_mouse_button_states[a_code] == input_state::up;
	}
}
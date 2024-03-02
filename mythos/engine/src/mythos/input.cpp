#include <mythos/event/key_event.hpp>
#include <mythos/input.hpp>

/// MYTODO: Would be preferable to have the query_lockable_key_states in the platform folder
#ifdef MYL_OS_WINDOWS
#   include <Windows.h> // WinUser.h
namespace myth {
    static auto query_toggleable_key_states(input::state* caps_lock, input::state* num_lock, input::state* scroll_lock) -> void {
        if (caps_lock)
            *caps_lock = (GetKeyState(VK_CAPITAL) & 0x0001) == 0 ? input::state::up : input::state::down;
        if (num_lock)
            *num_lock = (GetKeyState(VK_NUMLOCK) & 0x0001) == 0 ? input::state::up : input::state::down;
        if (scroll_lock)
            *scroll_lock = (GetKeyState(VK_SCROLL) & 0x0001) == 0 ? input::state::up : input::state::down;
    }
}
#else
#   include <mythos/log.hpp>
namespace myth {
    static auto query_toggleable_key_states(input::state* caps_lock, input::state* num_lock, input::state* scroll_lock) -> void {
        MYTHOS_WARN("Mythos does not support querying toggleable key states on this platform");
    }
}
#endif

namespace myth {
    std::array<input::state, key::size> input::s_key_states;

    auto input::init() -> void {
        clear();
        query_toggleable_key_states(&s_key_states[key::caps_lock], &s_key_states[key::num_lock], &s_key_states[key::scroll_lock]);
    }

    auto input::update() -> void {

    }

    auto input::clear() -> void {
        s_key_states.fill(state::up);
    }

    auto input::process_key(keycode code, state state) -> void {
        if (code == key::unknown)
            return;

        if (s_key_states[code] != state) { // Pressed (Non held) and released events should only be fired on key state change
            s_key_states[code] = state;

            if (state == state::up) {
                event::key_released e(code);
                event::fire(e);
            }
            else {
                event::key_pressed e(code, false);
                event::fire(e);
            }
        }
        else if (state == state::down) { // Key is held down
            event::key_pressed e(code, true);
            event::fire(e);
        }
    }

    auto input::process_typed(myl::u16 character) -> void {
        /// MYTODO: Allow a wider ranger of characters
        if (character < 32 || (character > 126 && character < 160))
			return;
		event::typed e(character);
		event::fire(e);
    }
}

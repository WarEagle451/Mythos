#include <mythos/event/key_event.hpp>
#include <mythos/event/mouse_event.hpp>
#include <mythos/input.hpp>

/// MYTODO: Split this up into seperate source files
#ifdef MYL_OS_WINDOWS
#   include <Windows.h> // WinUser.h

namespace myth {
    auto input::set_cursor_position(const myl::i32vec2& position) -> void {
        SetCursorPos(position.x, position.y);
    }

    auto input::set_cursor_visability(bool visable) -> void {
        ShowCursor(visable);
    }

    auto input::confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void {
        RECT rect{
            .left = tl.x,
            .top = tl.y,
            .right = br.x,
            .bottom = br.y
        };
        ClipCursor(&rect);
    }

    auto input::unconfine_cursor() -> void {
        ClipCursor(NULL);
    }

    auto input::query_toggleable_key_states(input::state* caps_lock, input::state* num_lock, input::state* scroll_lock) -> void {
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
    auto input::set_cursor_position(const myl::i32vec2& position) -> void {
        MYTHOS_ERROR("myth::input::set_cursor_position is not available on this platform");
    }

    auto input::set_cursor_visability(bool visable) -> void {
        MYTHOS_ERROR("myth::input::set_cursor_visability is not available on this platform");
    }

    auto input::confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void {
        MYTHOS_ERROR("myth::input::confine_cursor is not available on this platform");
    }

    auto input::unconfine_cursor() -> void {
        MYTHOS_ERROR("myth::input::unconfine_cursor is not available on this platform");
    }

    auto input::query_toggleable_key_states(input::state* caps_lock, input::state* num_lock, input::state* scroll_lock) -> void {
        MYTHOS_ERROR("myth::input::query_toggleable_key_states is not available on this platform");    
    }
}
#endif

namespace myth {
    std::array<input::state, key::size> input::s_key_states;
    mousecode input::s_mouse_button_states;

    myl::f32vec2 input::s_window_cursor_position;
    myl::f32vec2 input::s_cursor_delta;
    myl::f32vec2 input::s_scroll_delta;

    auto input::init() -> void {
        clear();
        /// MYTODO: Should probably query everything that is not a delta
        query_toggleable_key_states(&s_key_states[key::caps_lock], &s_key_states[key::num_lock], &s_key_states[key::scroll_lock]);
    }

    auto input::update() -> void {

    }

    auto input::clear() -> void {
        s_key_states.fill(state::up);
        s_mouse_button_states = mouse_button::none;

        s_window_cursor_position = { 0.f, 0.f };
        s_cursor_delta = { 0.f, 0.f };
        s_scroll_delta = { 0.f, 0.f };
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

    auto input::process_mouse_buttons_up(mousecode code) -> void {
        // new = 0110; old = 1010
        // old & new = 0010. Therefore bit 2 has changed state to up
        mousecode changed_buttons = s_mouse_button_states & code;
        if (changed_buttons != 0) { // Only update if there is a change
            s_mouse_button_states &= ~changed_buttons; // Clear bits to up
            event::mouse_released e(changed_buttons);
            event::fire(e);
        }
    }

    auto input::process_mouse_buttons_down(mousecode code) -> void {
        // new = 0110; old = 1010
        // ~(old | ~new) = 0100. Therefore bit 3 has changed state to down
        mousecode changed_buttons = ~(s_mouse_button_states | ~code); // Getting changed down buttons
        if (changed_buttons != 0) { // Only update if there is a change
            s_mouse_button_states |= changed_buttons; // Set bits to down
            event::mouse_pressed e(changed_buttons);
            event::fire(e);
        }
    }

    auto input::process_mouse_wheel(const myl::f32vec2& delta) -> void {
        s_scroll_delta = delta;
        event::mouse_scrolled e(s_scroll_delta);
        event::fire(e);
    }

    auto input::process_cursor_delta(const myl::f32vec2& delta) -> void {
        // Only update if the cursor delta has changed
        if (delta.x != 0.f || delta.y != 0.f)
            s_cursor_delta = delta;
    }

    auto input::process_window_cursor_position(const myl::f32vec2& position) -> void {
        if (s_window_cursor_position != position) {
            s_window_cursor_position = position;
            event::mouse_moved e(s_window_cursor_position);
            event::fire(e);
        }
    }
}

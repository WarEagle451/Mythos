#include <mythos/platform/windows/win_input.hpp>

#ifdef MYL_OS_WINDOWS
#   include <Windows.h> // WinUser.h

namespace myth::win {
    auto set_cursor_position(const myl::i32vec2& position) -> void {
        SetCursorPos(position.x, position.y);
    }

    auto set_cursor_visibility(bool visible) -> void {
        CURSORINFO cursor_info{ .cbSize = sizeof(CURSORINFO) };
        GetCursorInfo(&cursor_info);
        if ((cursor_info.flags & CURSOR_SHOWING) != visible)
            ShowCursor(visible);
    }

    auto confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void {
        RECT rect{
            .left = tl.x,
            .top = tl.y,
            .right = br.x,
            .bottom = br.y
        };
        ClipCursor(&rect);
    }

    auto release_cursor() -> void {
        ClipCursor(NULL);
    }

    auto query_togglable_keys(keyboard* keyboard) -> void {
        /// MYTODO: Should probably query everything that is not a delta

        keyboard->keys[key::caps_lock]   = (GetKeyState(VK_CAPITAL) & 0x0001) == 0 ? keyboard::state::up : keyboard::state::down;
        keyboard->keys[key::num_lock]    = (GetKeyState(VK_NUMLOCK) & 0x0001) == 0 ? keyboard::state::up : keyboard::state::down;
        keyboard->keys[key::scroll_lock] = (GetKeyState(VK_SCROLL) & 0x0001) == 0 ? keyboard::state::up : keyboard::state::down;
    }
}
#endif
#pragma once
#include <mythos/input.hpp>

#ifdef MYL_OS_WINDOWS
#   include <Windows.h> // WinUser.h

namespace mye::win {
    auto set_cursor_position(const myl::i32vec2& position) -> void;
    auto set_cursor_visibility(bool visible) -> void;
    auto confine_cursor(const myl::i32vec2& tl, const myl::i32vec2& br) -> void;
    auto release_cursor() -> void;

    auto query_togglable_keys(keyboard_state* keyboard) -> void;
}
#endif

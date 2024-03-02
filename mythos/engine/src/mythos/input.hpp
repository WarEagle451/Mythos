#pragma once
#include <mythos/core/keycodes.hpp>

#include <array>

namespace myth {
    class input {
    public:
        enum class state : bool { up = false, down = true };
    private:
        static std::array<state, key::size> s_key_states;
    public:
        static auto init() -> void;
        static auto update() -> void;

        static auto clear() -> void;
        static auto process_key(keycode code, state state) -> void;
        static auto process_typed(myl::u16 character) -> void;
    };
}

#pragma once
#include <mythos/event/event.hpp>
#include <mythos/input/keycodes.hpp>

namespace myth::event {
    class key_pressed : public base {
        keycode m_code;
        bool m_repeat;
    public:
        MYL_NO_DISCARD constexpr key_pressed(const keycode code, bool repeat)
            : m_code{ code }
            , m_repeat{ repeat } {}

        MYL_NO_DISCARD constexpr auto key() const noexcept -> keycode { return m_code; }
        MYL_NO_DISCARD constexpr auto repeated() const noexcept -> bool { return m_repeat; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::keyboard | category_flags::input)
        MYTHOS_IMPL_EVENT_TYPE(key_pressed)
    };

    class key_released : public base {
        keycode m_code;
    public:
        MYL_NO_DISCARD constexpr key_released(const keycode code)
            : m_code{ code } {}

        MYL_NO_DISCARD constexpr auto key() const noexcept -> keycode { return m_code; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::keyboard | category_flags::input)
        MYTHOS_IMPL_EVENT_TYPE(key_released)
    };

    class typed : public base {
        myl::u16 m_character;
    public:
        MYL_NO_DISCARD constexpr typed(myl::u16 character)
            : m_character{ character } {}

        MYL_NO_DISCARD constexpr auto character() const noexcept -> myl::u16 { return m_character; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::keyboard | category_flags::input)
        MYTHOS_IMPL_EVENT_TYPE(key_typed)
    };
}

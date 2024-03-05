#pragma once
#include <mythos/event/event.hpp>
#include <mythos/input.hpp>

namespace myth::event {
    class gamepad_button_pressed : public base {
        button_code m_code;
    public:
        MYL_NO_DISCARD constexpr gamepad_button_pressed(button_code code)
            : m_code{ code } {}

        MYL_NO_DISCARD constexpr auto buttons() const noexcept -> button_code { return m_code; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::mouse | category_flags::input | category_flags::gamepad_button)
        MYTHOS_IMPL_EVENT_TYPE(gamepad_button_pressed)
    };

    class gamepad_button_released : public base {
        button_code m_code;
    public:
        MYL_NO_DISCARD constexpr gamepad_button_released(button_code code)
            : m_code{ code } {}

        MYL_NO_DISCARD constexpr auto buttons() const noexcept -> button_code { return m_code; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::mouse | category_flags::input | category_flags::gamepad_button)
        MYTHOS_IMPL_EVENT_TYPE(gamepad_button_released)
    };
}

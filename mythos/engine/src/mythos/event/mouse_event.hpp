#pragma once
#include <mythos/event/event.hpp>
#include <mythos/input/button.hpp>

#include <myl/math/vec2.hpp>

namespace mye::event {
    class mouse_moved : public base {
        myl::f32vec2 m_position;
    public:
        MYL_NO_DISCARD constexpr mouse_moved(const myl::f32vec2& position)
            : m_position{ position } {}

        MYL_NO_DISCARD constexpr auto position() const noexcept -> const myl::f32vec2& { return m_position; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::mouse | category_flags::input)
        MYTHOS_IMPL_EVENT_TYPE(mouse_moved)
    };

    class mouse_scrolled : public base {
        myl::f32vec2 m_delta;
    public:
        MYL_NO_DISCARD constexpr mouse_scrolled(const myl::f32 dx, const myl::f32 dy)
            : m_delta{ dx, dy } {}

        MYL_NO_DISCARD constexpr mouse_scrolled(const myl::f32vec2& delta)
            : m_delta{ delta } {}

        MYL_NO_DISCARD constexpr auto delta() const noexcept -> const myl::f32vec2& { return m_delta; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::mouse | category_flags::input)
        MYTHOS_IMPL_EVENT_TYPE(mouse_scrolled)
    };

    class mouse_pressed : public base {
        mousecode m_code;
    public:
        MYL_NO_DISCARD constexpr mouse_pressed(mousecode buttons)
            : m_code{ buttons } {}

        MYL_NO_DISCARD constexpr auto buttons() const noexcept -> mousecode { return m_code; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::mouse | category_flags::input | category_flags::mouse_button)
        MYTHOS_IMPL_EVENT_TYPE(mouse_pressed)
    };

    class mouse_released : public base {
        mousecode m_code;
    public:
        MYL_NO_DISCARD constexpr mouse_released(mousecode buttons)
            : m_code{ buttons } {}

        MYL_NO_DISCARD constexpr auto buttons() const noexcept -> mousecode { return m_code; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::mouse | category_flags::input | category_flags::mouse_button)
        MYTHOS_IMPL_EVENT_TYPE(mouse_released)
    };
}

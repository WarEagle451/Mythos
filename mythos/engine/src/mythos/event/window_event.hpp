#pragma once
#include <mythos/event/event.hpp>
#include <mythos/platform/window.hpp>

#include <myl/math/vec2.hpp>

namespace myth::event {
    class window_resize : public base {
        window& m_window;
        myl::i32vec2 m_dimensions;
    public:
        MYL_NO_DISCARD constexpr window_resize(window& window, myl::i32 w, myl::i32 h)
            : m_window{ window }
            , m_dimensions{ w, h } {}
        MYL_NO_DISCARD constexpr window_resize(window& window, const myl::i32vec2& dimensions)
            : m_window{ window }
            , m_dimensions(dimensions) {}

        MYL_NO_DISCARD constexpr auto dim() const -> const myl::i32vec2& { return m_dimensions; }
        MYL_NO_DISCARD constexpr auto window() -> window& { return m_window; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::application)
        MYTHOS_IMPL_EVENT_TYPE(window_resize)
    };

    class window_close : public base {
        window& m_window;
    public:
        MYL_NO_DISCARD constexpr window_close(window& window)
            : m_window{ window } {}

        MYL_NO_DISCARD constexpr auto window() -> window& { return m_window; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::application)
        MYTHOS_IMPL_EVENT_TYPE(window_close)
    };

    class window_focus_gain : public base {
        window& m_window;
    public:
        MYL_NO_DISCARD constexpr window_focus_gain(window& window)
            : m_window{ window } {}

        MYL_NO_DISCARD constexpr auto window() -> window& { return m_window; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::application)
        MYTHOS_IMPL_EVENT_TYPE(window_focus_gain)
    };

    class window_focus_lost : public base {
        window& m_window;
    public:
        MYL_NO_DISCARD constexpr window_focus_lost(window& window)
            : m_window{ window } {}

        MYL_NO_DISCARD auto window() -> window& { return m_window; }

        MYTHOS_IMPL_EVENT_CATEGORY(category_flags::application)
        MYTHOS_IMPL_EVENT_TYPE(window_focus_lost)
    };
}

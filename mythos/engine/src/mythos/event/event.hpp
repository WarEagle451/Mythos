#pragma once
#include <myl/definitions.hpp>

#include <functional>
#include <type_traits>

#define MYTHOS_BIND_EVENT_FUNC(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace myth::event {
    enum class type {
        none,

        window_close,
        window_resize,
        window_focus_gain,
        window_focus_lost,
        window_moved,

        application_tick,
        application_update,
        application_render,

        key_pressed,
        key_released,
        key_typed,

        mouse_pressed,
        mouse_released,
        mouse_moved,
        mouse_scrolled,

        gamepad_button_pressed,
        gamepad_button_released,
    };

    namespace category_flags {
        enum bits : myl::u32 {
            none           = 0,
            application    = 1 << 0,
            input          = 1 << 1,
            keyboard       = 1 << 2,
            mouse          = 1 << 3,
            mouse_button   = 1 << 4,
            gamepad_button = 1 << 5
        };
    }
    using category = typename std::underlying_type<category_flags::bits>::type;

    class base {
    public:
        bool handled = false;

        constexpr virtual ~base() = default;

        MYL_NO_DISCARD constexpr virtual auto type() const -> type = 0;
        MYL_NO_DISCARD constexpr virtual auto name() const -> const char* = 0;
        MYL_NO_DISCARD constexpr virtual auto category_flags() const -> category = 0;

        MYL_NO_DISCARD constexpr auto is_in_category(category flags) const -> bool { return category_flags() & flags; }
    };

    class dispatcher {
        base& m_event;
    public:
        MYL_NO_DISCARD constexpr dispatcher(base& e)
            : m_event{ e } {}

        template<typename Event, typename Func>
        auto dispatch(const Func& func) -> bool {
            if (m_event.type() == Event::static_type()) {
                m_event.handled |= func(static_cast<Event&>(m_event));
                return true;
            }

            return false;
        }
    };

    using callback = std::function<void(base&)>;

    auto set_callback(callback& c) -> void;
    auto fire(base& e) -> void;
}

#define MYTHOS_IMPL_EVENT_CATEGORY(c) MYL_NO_DISCARD constexpr virtual auto category_flags() const -> myth::event::category override { return c; }
#define MYTHOS_IMPL_EVENT_TYPE(t)\
    MYL_NO_DISCARD static constexpr auto static_type() -> myth::event::type { return myth::event::type::t; }\
    MYL_NO_DISCARD constexpr virtual auto type() const -> myth::event::type override { return static_type(); }\
    MYL_NO_DISCARD constexpr virtual auto name() const -> const char* override { return #t; }
